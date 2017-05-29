#include <cstdint>
#include <type_traits>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <canard/mac_address.hpp>
#include <canard/packet_parser.hpp>
#include <canard/net/ofp/v10/common/match.hpp>
#include <canard/net/ofp/v10/common/match_fields.hpp>
#include <canard/net/ofp/v10/openflow.hpp>

struct match_creator
{
    template <class ByteRange>
    static auto to_address_v4(ByteRange const& range)
        -> boost::asio::ip::address_v4
    {
        auto bytes = boost::asio::ip::address_v4::bytes_type{};
        boost::copy(range, bytes.begin());
        return boost::asio::ip::address_v4{bytes};
    }

    bool operator()(canard::ether_header const& ether) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::eth_dst{ether.destination()});
        match.set(fields::eth_src{ether.source()});
        match.set(fields::eth_type{ether.ether_type()});
        return true;
    }

    bool operator()(canard::vlan_tag const& vlan) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::vlan_vid(vlan.vid()));
        match.set(fields::vlan_pcp{vlan.pcp()});
        // match.set(fields::eth_type{vlan.ether_type()});
        return true;
    }

    bool operator()(canard::arp const& arp) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::arp_spa(to_address_v4(arp.sender_protocol_address())));
        match.set(fields::arp_tpa(to_address_v4(arp.target_protocol_address())));
        return true;
    }

    bool operator()(canard::ipv4_header const& ipv4) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        // match.set(fields::ipv4_tos{ipv4.dscp()});
        match.set(fields::ip_proto{ipv4.protocol()});
        match.set(fields::ipv4_src(ipv4.source_address()));
        match.set(fields::ipv4_dst(ipv4.destination_address()));
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
        return false;
    }

    bool operator()(canard::ipv6_extension_header const& exthdr) const
    {
        return false;
    }

    bool operator()(canard::ipv6_fragment_header const& exthdr) const
    {
        return false;
    }

    bool operator()(canard::icmpv4 const& icmpv4) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::icmpv4_type{icmpv4.type()});
        match.set(fields::icmpv4_code{icmpv4.code()});
        return true;
    }

    bool operator()(canard::icmpv6 const& icmpv6) const
    {
        return false;
    }

    bool operator()(canard::icmpv6_neighbor_solicitation const& icmpv6) const
    {
        return false;
    }

    bool operator()(canard::icmpv6_neighbor_advertisement const& icmpv6) const
    {
        return false;
    }

    bool operator()(canard::udp_header const& udp) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::udp_src{udp.source_port()});
        match.set(fields::udp_dst{udp.destination_port()});
        return true;
    }

    bool operator()(canard::tcp_header const& tcp) const
    {
        namespace fields = canard::net::ofp::v10::match_fields;
        match.set(fields::tcp_src{tcp.source_port()});
        match.set(fields::tcp_dst{tcp.destination_port()});
        return true;
    }

    template <class T, typename std::enable_if<!std::is_base_of<canard::icmpv4, T>::value && !std::is_base_of<canard::icmpv6, T>::value>::type* = nullptr>
    bool operator()(T const&) const
    {
        return true;
    }

    canard::net::ofp::v10::match& match;
};

template <class Range>
auto match_from_packet(Range const& packet, std::uint16_t const in_port = 0)
    -> canard::net::ofp::v10::match
{
    namespace v10 = canard::net::ofp::v10;
    auto match = v10::match{};
    if (in_port != 0) {
        match.set(v10::match_fields::in_port{in_port});
    }
    canard::for_each_header(packet, match_creator{match});
    return match;
}

