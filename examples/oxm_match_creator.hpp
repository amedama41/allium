#include <cstdint>
#include <type_traits>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <canard/mac_address.hpp>
#include <canard/packet_parser.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

struct oxm_match_creator
{
    template <class ByteRange>
    static auto to_address_v4(ByteRange const& range)
        -> boost::asio::ip::address_v4
    {
        auto bytes = boost::asio::ip::address_v4::bytes_type{};
        boost::copy(range, bytes.begin());
        return boost::asio::ip::address_v4{bytes};
    }

    template <class ByteRange>
    static auto to_mac_address(ByteRange const& range)
        -> canard::mac_address
    {
        auto array = std::array<std::uint8_t, 6>{};
        boost::copy(range, array.begin());
        return canard::mac_address{array};
    }

    bool operator()(canard::ether_header const& ether) const
    {
        match.add(canard::network::openflow::v13::oxm_eth_dst{ether.destination()});
        match.add(canard::network::openflow::v13::oxm_eth_src{ether.source()});
        if (ether.ether_type() != ETHERTYPE_VLAN) {
            match.add(canard::network::openflow::v13::oxm_eth_type{ether.ether_type()});
        }
        return true;
    }

    bool operator()(canard::vlan_tag const& vlan) const
    {
        match.add(canard::network::openflow::v13::oxm_vlan_vid(vlan.vid()));
        match.add(canard::network::openflow::v13::oxm_vlan_pcp{vlan.pcp()});
        match.add(canard::network::openflow::v13::oxm_eth_type{vlan.ether_type()});
        return true;
    }

    bool operator()(canard::arp const& arp) const
    {
        match.add(canard::network::openflow::v13::oxm_arp_op{arp.operation()});
        match.add(canard::network::openflow::v13::oxm_arp_spa(to_address_v4(arp.sender_protocol_address()).to_ulong()));
        match.add(canard::network::openflow::v13::oxm_arp_tpa(to_address_v4(arp.target_protocol_address()).to_ulong()));
        if (arp.hardware_type() == 1) {
            match.add(canard::network::openflow::v13::oxm_arp_sha{to_mac_address(arp.sender_hardware_address())});
            match.add(canard::network::openflow::v13::oxm_arp_tha{to_mac_address(arp.target_hardware_address())});
        }
        return true;
    }

    bool operator()(canard::ipv4_header const& ipv4) const
    {
        match.add(canard::network::openflow::v13::oxm_ip_dscp{ipv4.dscp()});
        match.add(canard::network::openflow::v13::oxm_ip_ecn{ipv4.ecn()});
        match.add(canard::network::openflow::v13::oxm_ip_proto{ipv4.protocol()});
        match.add(canard::network::openflow::v13::oxm_ipv4_src(ipv4.source().to_ulong()));
        match.add(canard::network::openflow::v13::oxm_ipv4_dst(ipv4.destination().to_ulong()));
        return true;
    }

    static bool is_extension_header(std::uint8_t const next_header)
    {
        switch (next_header) {
        case 0: case 43: case 46: case 51: case 60:
            return true;
        }
        return false;
    }

    bool operator()(canard::ipv6_header const& ipv6) const
    {
        match.add(canard::network::openflow::v13::oxm_ip_dscp{ipv6.dscp()});
        match.add(canard::network::openflow::v13::oxm_ip_ecn{ipv6.ecn()});
        match.add(canard::network::openflow::v13::oxm_ipv6_flabel{ipv6.flow_label()});
        if (not is_extension_header(ipv6.next_header())) {
            match.add(canard::network::openflow::v13::oxm_ip_proto{ipv6.next_header()});
        }
        match.add(canard::network::openflow::v13::oxm_ipv6_src(ipv6.source()));
        match.add(canard::network::openflow::v13::oxm_ipv6_dst(ipv6.destination()));
        return true;
    }

    bool operator()(canard::ipv6_extension_header const& exthdr) const
    {
        if (not is_extension_header(exthdr.next_header())) {
            match.add(canard::network::openflow::v13::oxm_ip_proto{exthdr.next_header()});
        }
        return true;
    }

    bool operator()(canard::ipv6_fragment_header const& exthdr) const
    {
        if (not is_extension_header(exthdr.next_header())) {
            match.add(canard::network::openflow::v13::oxm_ip_proto{exthdr.next_header()});
        }
        return true;
    }

    bool operator()(canard::icmpv4 const& icmpv4) const
    {
        match.add(canard::network::openflow::v13::oxm_icmpv4_type{icmpv4.type()});
        match.add(canard::network::openflow::v13::oxm_icmpv4_code{icmpv4.code()});
        return true;
    }

    bool operator()(canard::icmpv6 const& icmpv6) const
    {
        match.add(canard::network::openflow::v13::oxm_icmpv6_type{icmpv6.type()});
        match.add(canard::network::openflow::v13::oxm_icmpv6_code{icmpv6.code()});
        return true;
    }

    bool operator()(canard::icmpv6_neighbor_solicitation const& icmpv6) const
    {
        match.add(canard::network::openflow::v13::oxm_icmpv6_type{icmpv6.type()});
        match.add(canard::network::openflow::v13::oxm_icmpv6_code{icmpv6.code()});
        match.add(canard::network::openflow::v13::oxm_ipv6_nd_target{icmpv6.target_address()});
        if (boost::distance(icmpv6.source_link_layer_address()) == 6) {
            match.add(canard::network::openflow::v13::oxm_ipv6_nd_sll{to_mac_address(icmpv6.source_link_layer_address())});
        }
        return true;
    }

    bool operator()(canard::icmpv6_neighbor_advertisement const& icmpv6) const
    {
        match.add(canard::network::openflow::v13::oxm_icmpv6_type{icmpv6.type()});
        match.add(canard::network::openflow::v13::oxm_icmpv6_code{icmpv6.code()});
        match.add(canard::network::openflow::v13::oxm_ipv6_nd_target{icmpv6.target_address()});
        if (boost::distance(icmpv6.target_link_layer_address()) == 6) {
            match.add(canard::network::openflow::v13::oxm_ipv6_nd_tll{to_mac_address(icmpv6.target_link_layer_address())});
        }
        return true;
    }

    bool operator()(canard::udp_header const& udp) const
    {
        match.add(canard::network::openflow::v13::oxm_udp_src{udp.source()});
        match.add(canard::network::openflow::v13::oxm_udp_dst{udp.destination()});
        return true;
    }

    bool operator()(canard::tcp_header const& tcp) const
    {
        match.add(canard::network::openflow::v13::oxm_tcp_src{tcp.source()});
        match.add(canard::network::openflow::v13::oxm_tcp_dst{tcp.destination()});
        return true;
    }

    template <class T, typename std::enable_if<!std::is_base_of<canard::icmpv4, T>::value && !std::is_base_of<canard::icmpv6, T>::value>::type* = nullptr>
    bool operator()(T const&) const
    {
        return true;
    }

    canard::network::openflow::v13::oxm_match& match;
};

auto oxm_match_from_packet(std::vector<unsigned char> const& packet)
    -> canard::network::openflow::v13::oxm_match
{
    auto match = canard::network::openflow::v13::oxm_match{};
    canard::for_each_header(packet, oxm_match_creator{match});
    return match;
}

