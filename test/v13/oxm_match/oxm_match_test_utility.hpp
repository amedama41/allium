#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_TEST_UTILITY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_TEST_UTILITY_HPP

#include <canard/network/protocol/openflow/v13/openflow.hpp>

#define OXM_HEADER__(CLASS, FIELD, HASMASK, LENGTH) \
    (((CLASS) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
#define OXM_HEADER(CLASS, FIELD, LENGTH) \
    OXM_HEADER__(CLASS, FIELD, 0, LENGTH)
#define OXM_HEADER_W(CLASS, FIELD, LENGTH) \
    OXM_HEADER__(CLASS, FIELD, 1, (LENGTH) * 2)
#define OXM_TYPE(HEADER) (((HEADER) >> 9) & 0x7fffff)

namespace {


constexpr auto OXM_OF_IN_PORT
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IN_PORT, 4);
constexpr auto OXM_OF_IN_PORT_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IN_PORT, 4);
constexpr auto OXM_OF_IN_PHY_PORT
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IN_PHY_PORT, 4);
constexpr auto OXM_OF_IN_PHY_PORT_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IN_PHY_PORT, 4);
constexpr auto OXM_OF_METADATA
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_METADATA, 8);
constexpr auto OXM_OF_METADATA_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_METADATA, 8);
constexpr auto OXM_OF_ETH_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_DST, 6);
constexpr auto OXM_OF_ETH_DST_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_DST, 6);
constexpr auto OXM_OF_ETH_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_SRC, 6);
constexpr auto OXM_OF_ETH_SRC_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_SRC, 6);
constexpr auto OXM_OF_ETH_TYPE
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_TYPE, 2);
constexpr auto OXM_OF_ETH_TYPE_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ETH_TYPE, 2);
constexpr auto OXM_OF_VLAN_VID
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_VLAN_VID, 2);
constexpr auto OXM_OF_VLAN_VID_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_VLAN_VID, 2);
constexpr auto OXM_OF_VLAN_PCP
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_VLAN_PCP, 1);
constexpr auto OXM_OF_VLAN_PCP_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_VLAN_PCP, 1);
constexpr auto OXM_OF_IP_DSCP
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_DSCP, 1);
constexpr auto OXM_OF_IP_DSCP_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_DSCP, 1);
constexpr auto OXM_OF_IP_ECN
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_ECN, 1);
constexpr auto OXM_OF_IP_ECN_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_ECN, 1);
constexpr auto OXM_OF_IP_PROTO
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_PROTO, 1);
constexpr auto OXM_OF_IP_PROTO_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IP_PROTO, 1);
constexpr auto OXM_OF_IPV4_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV4_SRC, 4);
constexpr auto OXM_OF_IPV4_SRC_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV4_SRC, 4);
constexpr auto OXM_OF_IPV4_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV4_DST, 4);
constexpr auto OXM_OF_IPV4_DST_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV4_DST, 4);
constexpr auto OXM_OF_TCP_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TCP_SRC, 2);
constexpr auto OXM_OF_TCP_SRC_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TCP_SRC, 2);
constexpr auto OXM_OF_TCP_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TCP_DST, 2);
constexpr auto OXM_OF_TCP_DST_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TCP_DST, 2);
constexpr auto OXM_OF_UDP_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_UDP_SRC, 2);
constexpr auto OXM_OF_UDP_SRC_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_UDP_SRC, 2);
constexpr auto OXM_OF_UDP_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_UDP_DST, 2);
constexpr auto OXM_OF_UDP_DST_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_UDP_DST, 2);
constexpr auto OXM_OF_SCTP_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_SCTP_SRC, 2);
constexpr auto OXM_OF_SCTP_SRC_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_SCTP_SRC, 2);
constexpr auto OXM_OF_SCTP_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_SCTP_DST, 2);
constexpr auto OXM_OF_SCTP_DST_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_SCTP_DST, 2);
constexpr auto OXM_OF_ICMPV4_TYPE
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV4_TYPE, 1);
constexpr auto OXM_OF_ICMPV4_TYPE_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV4_TYPE, 1);
constexpr auto OXM_OF_ICMPV4_CODE
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV4_CODE, 1);
constexpr auto OXM_OF_ICMPV4_CODE_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV4_CODE, 1);
constexpr auto OXM_OF_ARP_OP
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_OP, 2);
constexpr auto OXM_OF_ARP_OP_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_OP, 2);
constexpr auto OXM_OF_ARP_SPA
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_SPA, 4);
constexpr auto OXM_OF_ARP_SPA_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_SPA, 4);
constexpr auto OXM_OF_ARP_TPA
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_TPA, 4);
constexpr auto OXM_OF_ARP_TPA_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_TPA, 4);
constexpr auto OXM_OF_ARP_SHA
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_SHA, 6);
constexpr auto OXM_OF_ARP_SHA_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_SHA, 6);
constexpr auto OXM_OF_ARP_THA
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_THA, 6);
constexpr auto OXM_OF_ARP_THA_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ARP_THA, 6);
constexpr auto OXM_OF_IPV6_SRC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_SRC, 16);
constexpr auto OXM_OF_IPV6_SRC_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_SRC, 16);
constexpr auto OXM_OF_IPV6_DST
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_DST, 16);
constexpr auto OXM_OF_IPV6_DST_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_DST, 16);
constexpr auto OXM_OF_IPV6_FLABEL
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_FLABEL, 4);
constexpr auto OXM_OF_IPV6_FLABEL_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_FLABEL, 4);
constexpr auto OXM_OF_ICMPV6_TYPE
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV6_TYPE, 1);
constexpr auto OXM_OF_ICMPV6_TYPE_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV6_TYPE, 1);
constexpr auto OXM_OF_ICMPV6_CODE
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV6_CODE, 1);
constexpr auto OXM_OF_ICMPV6_CODE_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_ICMPV6_CODE, 1);
constexpr auto OXM_OF_IPV6_ND_TARGET
    = OXM_HEADER (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_TARGET, 16);
constexpr auto OXM_OF_IPV6_ND_TARGET_W
    = OXM_HEADER (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_TARGET, 16);
constexpr auto OXM_OF_IPV6_ND_SLL
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_SLL, 6);
constexpr auto OXM_OF_IPV6_ND_SLL_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_SLL, 6);
constexpr auto OXM_OF_IPV6_ND_TLL
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_TLL, 6);
constexpr auto OXM_OF_IPV6_ND_TLL_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_ND_TLL, 6);
constexpr auto OXM_OF_MPLS_LABEL
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_LABEL, 4);
constexpr auto OXM_OF_MPLS_LABEL_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_LABEL, 4);
constexpr auto OXM_OF_MPLS_TC
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_TC, 1);
constexpr auto OXM_OF_MPLS_TC_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_TC, 1);
constexpr auto OXM_OF_MPLS_BOS
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_BOS, 1);
constexpr auto OXM_OF_MPLS_BOS_W
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_MPLS_BOS, 1);
constexpr auto OXM_OF_PBB_ISID
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_PBB_ISID, 3);
constexpr auto OXM_OF_PBB_ISID_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_PBB_ISID, 3);
constexpr auto OXM_OF_TUNNEL_ID
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TUNNEL_ID, 8);
constexpr auto OXM_OF_TUNNEL_ID_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_TUNNEL_ID, 8);
constexpr auto OXM_OF_IPV6_EXTHDR
    = OXM_HEADER  (0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_EXTHDR, 2);
constexpr auto OXM_OF_IPV6_EXTHDR_W
    = OXM_HEADER_W(0x8000, canard::network::openflow::v13::protocol::OFPXMT_OFB_IPV6_EXTHDR, 2);

}

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_TEST_UTILITY_HPP
