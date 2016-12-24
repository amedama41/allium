#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/mac_address.hpp>
#include <canard/packet_parser.hpp>
#include <canard/network/openflow/v13/common/oxm_match.hpp>
#include <canard/network/openflow/v13/common/oxm_match_field.hpp>
#include <canard/network/openflow/v13/openflow.hpp>

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

    bool operator()(canard::ether_header const& ether)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::eth_dst{ether.destination()});
        oxm_fields.push_back(fields::eth_src{ether.source()});
        if (ether.ether_type() != ETHERTYPE_VLAN) {
            oxm_fields.push_back(fields::eth_type{ether.ether_type()});
        }
        return true;
    }

    bool operator()(canard::vlan_tag const& vlan)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::vlan_vid(vlan.vid()));
        oxm_fields.push_back(fields::vlan_pcp{vlan.pcp()});
        oxm_fields.push_back(fields::eth_type{vlan.ether_type()});
        return true;
    }

    bool operator()(canard::arp const& arp)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::arp_op{arp.operation()});
        oxm_fields.push_back(fields::arp_spa(to_address_v4(arp.sender_protocol_address())));
        oxm_fields.push_back(fields::arp_tpa(to_address_v4(arp.target_protocol_address())));
        if (arp.hardware_type() == 1) {
            oxm_fields.push_back(fields::arp_sha{to_mac_address(arp.sender_hardware_address())});
            oxm_fields.push_back(fields::arp_tha{to_mac_address(arp.target_hardware_address())});
        }
        return true;
    }

    bool operator()(canard::ipv4_header const& ipv4)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::ip_dscp{ipv4.dscp()});
        oxm_fields.push_back(fields::ip_ecn{ipv4.ecn()});
        oxm_fields.push_back(fields::ip_proto{ipv4.protocol()});
        oxm_fields.push_back(fields::ipv4_src(ipv4.source()));
        oxm_fields.push_back(fields::ipv4_dst(ipv4.destination()));
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

    bool operator()(canard::ipv6_header const& ipv6)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::ip_dscp{ipv6.dscp()});
        oxm_fields.push_back(fields::ip_ecn{ipv6.ecn()});
        oxm_fields.push_back(fields::ipv6_flabel{ipv6.flow_label()});
        if (not is_extension_header(ipv6.next_header())) {
            oxm_fields.push_back(fields::ip_proto{ipv6.next_header()});
        }
        oxm_fields.push_back(fields::ipv6_src(ipv6.source()));
        oxm_fields.push_back(fields::ipv6_dst(ipv6.destination()));
        return true;
    }

    bool operator()(canard::ipv6_extension_header const& exthdr)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        if (not is_extension_header(exthdr.next_header())) {
            oxm_fields.push_back(fields::ip_proto{exthdr.next_header()});
        }
        return true;
    }

    bool operator()(canard::ipv6_fragment_header const& exthdr)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        if (not is_extension_header(exthdr.next_header())) {
            oxm_fields.push_back(fields::ip_proto{exthdr.next_header()});
        }
        return true;
    }

    bool operator()(canard::icmpv4 const& icmpv4)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::icmpv4_type{icmpv4.type()});
        oxm_fields.push_back(fields::icmpv4_code{icmpv4.code()});
        return true;
    }

    bool operator()(canard::icmpv6 const& icmpv6)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::icmpv6_type{icmpv6.type()});
        oxm_fields.push_back(fields::icmpv6_code{icmpv6.code()});
        return true;
    }

    bool operator()(canard::icmpv6_neighbor_solicitation const& icmpv6)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::icmpv6_type{icmpv6.type()});
        oxm_fields.push_back(fields::icmpv6_code{icmpv6.code()});
        oxm_fields.push_back(fields::ipv6_nd_target{icmpv6.target_address()});
        if (boost::distance(icmpv6.source_link_layer_address()) == 6) {
            oxm_fields.push_back(fields::ipv6_nd_sll{to_mac_address(icmpv6.source_link_layer_address())});
        }
        return true;
    }

    bool operator()(canard::icmpv6_neighbor_advertisement const& icmpv6)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::icmpv6_type{icmpv6.type()});
        oxm_fields.push_back(fields::icmpv6_code{icmpv6.code()});
        oxm_fields.push_back(fields::ipv6_nd_target{icmpv6.target_address()});
        if (boost::distance(icmpv6.target_link_layer_address()) == 6) {
            oxm_fields.push_back(fields::ipv6_nd_tll{to_mac_address(icmpv6.target_link_layer_address())});
        }
        return true;
    }

    bool operator()(canard::udp_header const& udp)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::udp_src{udp.source()});
        oxm_fields.push_back(fields::udp_dst{udp.destination()});
        return true;
    }

    bool operator()(canard::tcp_header const& tcp)
    {
        namespace fields = canard::net::ofp::v13::oxm_match_fields;
        oxm_fields.push_back(fields::tcp_src{tcp.source()});
        oxm_fields.push_back(fields::tcp_dst{tcp.destination()});
        return true;
    }

    template <class T, typename std::enable_if<!std::is_base_of<canard::icmpv4, T>::value && !std::is_base_of<canard::icmpv6, T>::value>::type* = nullptr>
    bool operator()(T const&) const
    {
        return true;
    }

    auto match()
        -> canard::net::ofp::v13::oxm_match
    {
        return canard::net::ofp::v13::oxm_match{std::move(oxm_fields)};
    }

    canard::net::ofp::v13::oxm_match::oxm_fields_type oxm_fields;
};

auto oxm_match_from_packet(boost::iterator_range<unsigned char const*> packet)
    -> canard::net::ofp::v13::oxm_match
{
    auto creator = oxm_match_creator{};
    canard::for_each_header(packet, creator);
    return creator.match();
}

