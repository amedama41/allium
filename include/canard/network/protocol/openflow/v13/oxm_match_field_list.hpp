#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_LIST_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using default_oxm_match_field_list = std::tuple<
          oxm_in_port
        , oxm_in_phy_port
        , oxm_metadata
        , oxm_eth_dst
        , oxm_eth_src
        , oxm_eth_type
        , oxm_vlan_vid
        , oxm_vlan_pcp
        , oxm_ip_dscp
        , oxm_ip_ecn
        , oxm_ip_proto
        , oxm_ipv4_src
        , oxm_ipv4_dst
        , oxm_tcp_src
        , oxm_tcp_dst
        , oxm_udp_src
        , oxm_udp_dst
        , oxm_sctp_src
        , oxm_sctp_dst
        , oxm_icmpv4_type
        , oxm_icmpv4_code
        , oxm_arp_op
        , oxm_arp_spa
        , oxm_arp_tpa
        , oxm_arp_sha
        , oxm_arp_tha
        , oxm_ipv6_src
        , oxm_ipv6_dst
        , oxm_ipv6_flabel
        , oxm_icmpv6_type
        , oxm_icmpv6_code
        , oxm_ipv6_nd_target
        , oxm_ipv6_nd_sll
        , oxm_ipv6_nd_tll
        , oxm_mpls_label
        , oxm_mpls_tc
        , oxm_mpls_bos
        , oxm_pbb_isid
        , oxm_tunnel_id
        , oxm_ipv6_exthdr
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_LIST_HPP
