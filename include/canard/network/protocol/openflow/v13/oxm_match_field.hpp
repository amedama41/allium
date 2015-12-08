#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/format.hpp>
#include <boost/optional/optional.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/oxm_type_definition_map.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace match {

    template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
    class oxm_match_field
        : public v13_detail::basic_oxm_type<OXMClass, OXMField>
    {
        using base_type = v13_detail::basic_oxm_type<OXMClass, OXMField>;

    public:
        using value_type = typename base_type::value_type;
        using needs_endian_conversion = typename base_type::needs_endian_conversion;

        explicit oxm_match_field(value_type value)
            : value_(std::move(value))
            , mask_{boost::none}
        {
        }

        oxm_match_field(value_type value, value_type mask)
            : value_(std::move(value))
            , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
        {
        }

        template <
              class T
            , typename std::enable_if<
                       !std::is_scalar<typename std::remove_reference<T>::type>::value
                    && !canard::is_related<oxm_match_field, T>::value
              >::type* = nullptr
        >
        explicit oxm_match_field(T&& value)
            : value_(std::forward<T>(value).to_bytes())
            , mask_{boost::none}
        {
        }

        template <class T, typename std::enable_if<!std::is_scalar<T>::value>::type* = nullptr>
        oxm_match_field(T&& value, value_type mask)
            : value_(std::forward<T>(value).to_bytes())
            , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
        {
        }

        auto oxm_header() const
            -> std::uint32_t
        {
            return (base_type::oxm_type() << 9) | (std::uint32_t{oxm_has_mask()} << 8) | oxm_length();
        }

        auto oxm_has_mask() const
            -> bool
        {
            return static_cast<bool>(mask_);
        }

        auto oxm_length() const
            -> std::uint8_t
        {
            return oxm_has_mask() ? sizeof(value_type) * 2 : sizeof(value_type);
        }

        auto oxm_value() const
            -> typename std::conditional<std::is_scalar<value_type>::value, value_type, value_type const&>::type
        {
            return value_;
        }

        auto oxm_mask() const
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(oxm_header()) + oxm_length();
        }

        auto wildcard() const
            -> bool
        {
            return mask_ ? all_of(*mask_, 0x00) : false;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, oxm_header());
            detail::encode(container, value_, sizeof(value_), needs_endian_conversion{});
            return mask_ ? detail::encode(container, *mask_, sizeof(*mask_), needs_endian_conversion{}) : container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_match_field
        {
            auto const oxm_header = detail::decode<std::uint32_t>(first, last);
            auto const value = detail::decode<value_type>(first, last, sizeof(value_type), needs_endian_conversion{});
            if (oxm_header & 0x00000100) {
                auto const mask = detail::decode<value_type>(first, last, sizeof(value_type), needs_endian_conversion{});
                return oxm_match_field{value, mask};
            }
            return oxm_match_field{value};
        }

    private:
        static auto all_of(value_type const& mask, unsigned char value)
            -> bool
        {
            return boost::algorithm::all_of(canard::as_byte_range(mask)
                    , [=](unsigned char c) { return c == value; });
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT
    >::oxm_match_field(value_type value)
        : value_(value)
        , mask_{boost::none}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(port_no:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value)
        , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(port_no:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT
    >::oxm_match_field(value_type value)
        : value_(value)
        , mask_{boost::none}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw 3;
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value)
        , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw 3;
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_match_field(value_type value)
        : value_(value == protocol::OFPVID_NONE
                ? protocol::OFPVID_NONE : (value | protocol::OFPVID_PRESENT))
        , mask_(value == protocol::OFPVID_PRESENT
                ? boost::optional<value_type>{protocol::OFPVID_PRESENT} : boost::none)
    {
        if (value_ > 0x1FFF) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(vlan_vid:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value == protocol::OFPVID_NONE
                ? protocol::OFPVID_NONE : (value | protocol::OFPVID_PRESENT))
        , mask_((mask & 0xFFF) == 0xFFF
                ? boost::none : boost::optional<value_type>(mask | protocol::OFPVID_PRESENT))
    {
        if (value_ > 0x1FFF) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(vlan_vid:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline auto oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_value() const
        -> value_type
    {
        return value_ == protocol::OFPVID_PRESENT
            ? protocol::OFPVID_PRESENT : value_ & ~protocol::OFPVID_PRESENT;
    }

    using oxm_in_port       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT>;
    using oxm_in_phy_port   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT>;
    using oxm_metadata      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_METADATA>;
    using oxm_eth_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST>;
    using oxm_eth_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC>;
    using oxm_eth_type      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE>;
    using oxm_vlan_vid      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID>;
    using oxm_vlan_pcp      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP>;
    using oxm_ip_dscp       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP>;
    using oxm_ip_ecn        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN>;
    using oxm_ip_proto      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO>;
    using oxm_ipv4_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC>;
    using oxm_ipv4_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST>;
    using oxm_tcp_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC>;
    using oxm_tcp_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST>;
    using oxm_udp_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC>;
    using oxm_udp_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST>;
    using oxm_sctp_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC>;
    using oxm_sctp_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST>;
    using oxm_icmpv4_type   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_TYPE>;
    using oxm_icmpv4_code   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_CODE>;
    using oxm_arp_op        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_OP>;
    using oxm_arp_spa       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SPA>;
    using oxm_arp_tpa       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_TPA>;
    using oxm_arp_sha       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SHA>;
    using oxm_arp_tha       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_THA>;
    using oxm_ipv6_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_SRC>;
    using oxm_ipv6_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_DST>;
    using oxm_ipv6_flabel   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_FLABEL>;
    using oxm_icmpv6_type   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_TYPE>;
    using oxm_icmpv6_code   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_CODE>;
    using oxm_ipv6_nd_target= oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TARGET>;
    using oxm_ipv6_nd_sll   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_SLL>;
    using oxm_ipv6_nd_tll   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TLL>;
    using oxm_mpls_label    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_LABEL>;
    using oxm_mpls_tc       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_TC>;
    using oxm_mpls_bos      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_BOS>;
    using oxm_pbb_isid      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_PBB_ISID>;
    using oxm_tunnel_id     = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TUNNEL_ID>;
    using oxm_ipv6_exthdr   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_EXTHDR>;

} // namespace match

using oxm_in_port = match::oxm_in_port;
using oxm_in_phy_port = match::oxm_in_phy_port;
using oxm_metadata = match::oxm_metadata;
using oxm_eth_dst = match::oxm_eth_dst;
using oxm_eth_src = match::oxm_eth_src;
using oxm_eth_type = match::oxm_eth_type;
using oxm_vlan_vid = match::oxm_vlan_vid;
using oxm_vlan_pcp = match::oxm_vlan_pcp;
using oxm_ip_dscp = match::oxm_ip_dscp;
using oxm_ip_ecn = match::oxm_ip_ecn;
using oxm_ip_proto = match::oxm_ip_proto;
using oxm_ipv4_src = match::oxm_ipv4_src;
using oxm_ipv4_dst = match::oxm_ipv4_dst;
using oxm_tcp_src = match::oxm_tcp_src;
using oxm_tcp_dst = match::oxm_tcp_dst;
using oxm_udp_src = match::oxm_udp_src;
using oxm_udp_dst = match::oxm_udp_dst;
using oxm_sctp_src = match::oxm_sctp_src;
using oxm_sctp_dst = match::oxm_sctp_dst;
using oxm_icmpv4_type = match::oxm_icmpv4_type;
using oxm_icmpv4_code = match::oxm_icmpv4_code;
using oxm_arp_op = match::oxm_arp_op;
using oxm_arp_spa = match::oxm_arp_spa;
using oxm_arp_tpa = match::oxm_arp_tpa;
using oxm_arp_sha = match::oxm_arp_sha;
using oxm_arp_tha = match::oxm_arp_tha;
using oxm_ipv6_src = match::oxm_ipv6_src;
using oxm_ipv6_dst = match::oxm_ipv6_dst;
using oxm_ipv6_flabel = match::oxm_ipv6_flabel;
using oxm_icmpv6_type = match::oxm_icmpv6_type;
using oxm_icmpv6_code = match::oxm_icmpv6_code;
using oxm_ipv6_nd_target = match::oxm_ipv6_nd_target;
using oxm_ipv6_nd_sll = match::oxm_ipv6_nd_sll;
using oxm_ipv6_nd_tll = match::oxm_ipv6_nd_tll;
using oxm_mpls_label = match::oxm_mpls_label;
using oxm_mpls_tc = match::oxm_mpls_tc;
using oxm_mpls_bos = match::oxm_mpls_bos;
using oxm_pbb_isid = match::oxm_pbb_isid;
using oxm_tunnel_id = match::oxm_tunnel_id;
using oxm_ipv6_exthdr = match::oxm_ipv6_exthdr;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP
