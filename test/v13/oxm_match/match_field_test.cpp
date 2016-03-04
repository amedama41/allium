#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/endian/arithmetic.hpp>
#include <canard/mac_address.hpp>
#include "./oxm_match_test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;

using boost::asio::ip::address_v4;
using boost::asio::ip::address_v6;
using boost::endian::big_uint24_t;
using canard::mac_address;

BOOST_AUTO_TEST_SUITE(oxm_match_test)

BOOST_AUTO_TEST_SUITE(in_port_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::in_port;

        BOOST_TEST((std::is_same<sut::value_type, std::uint32_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IN_PORT);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IN_PORT));
    }

BOOST_AUTO_TEST_SUITE_END() // in_port_test

BOOST_AUTO_TEST_SUITE(in_phy_port_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::in_phy_port;

        BOOST_TEST((std::is_same<sut::value_type, std::uint32_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IN_PHY_PORT);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IN_PHY_PORT));
    }

BOOST_AUTO_TEST_SUITE_END() // in_phy_port_test

BOOST_AUTO_TEST_SUITE(metadata_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::metadata;

        BOOST_TEST((std::is_same<sut::value_type, std::uint64_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_METADATA);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_METADATA));
    }

BOOST_AUTO_TEST_SUITE_END() // metadata_test

BOOST_AUTO_TEST_SUITE(eth_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::eth_dst;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ETH_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ETH_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // eth_dst_test

BOOST_AUTO_TEST_SUITE(eth_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::eth_src;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ETH_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ETH_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // eth_src_test

BOOST_AUTO_TEST_SUITE(eth_type_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::eth_type;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ETH_TYPE);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ETH_TYPE));
    }

BOOST_AUTO_TEST_SUITE_END() // eth_type_test

BOOST_AUTO_TEST_SUITE(vlan_vid_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::vlan_vid;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_VLAN_VID);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_VLAN_VID));
    }

BOOST_AUTO_TEST_SUITE_END() // vlan_vid_test

BOOST_AUTO_TEST_SUITE(vlan_pcp_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::vlan_pcp;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_VLAN_PCP);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_VLAN_PCP));
    }

BOOST_AUTO_TEST_SUITE_END() // vlan_pcp_test

BOOST_AUTO_TEST_SUITE(ip_dscp_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ip_dscp;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IP_DSCP);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IP_DSCP));
    }

BOOST_AUTO_TEST_SUITE_END() // ip_dscp_test

BOOST_AUTO_TEST_SUITE(ip_ecn_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ip_ecn;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IP_ECN);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IP_ECN));
    }

BOOST_AUTO_TEST_SUITE_END() // ip_ecn_test

BOOST_AUTO_TEST_SUITE(ip_proto_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ip_proto;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IP_PROTO);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IP_PROTO));
    }

BOOST_AUTO_TEST_SUITE_END() // ip_proto_test

BOOST_AUTO_TEST_SUITE(ipv4_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv4_src;

        BOOST_TEST((std::is_same<sut::value_type, address_v4>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV4_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV4_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv4_src_test

BOOST_AUTO_TEST_SUITE(ipv4_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv4_dst;

        BOOST_TEST((std::is_same<sut::value_type, address_v4>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV4_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV4_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv4_dst_test

BOOST_AUTO_TEST_SUITE(tcp_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::tcp_src;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_TCP_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_TCP_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // tcp_src_test

BOOST_AUTO_TEST_SUITE(tcp_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::tcp_dst;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_TCP_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_TCP_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // tcp_dst_test

BOOST_AUTO_TEST_SUITE(udp_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::udp_src;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_UDP_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_UDP_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // udp_src_test

BOOST_AUTO_TEST_SUITE(udp_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::udp_dst;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_UDP_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_UDP_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // udp_dst_test

BOOST_AUTO_TEST_SUITE(sctp_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::sctp_src;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_SCTP_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_SCTP_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // sctp_src_test

BOOST_AUTO_TEST_SUITE(sctp_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::sctp_dst;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_SCTP_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_SCTP_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // sctp_dst_test

BOOST_AUTO_TEST_SUITE(icmpv4_type_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::icmpv4_type;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ICMPV4_TYPE);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ICMPV4_TYPE));
    }

BOOST_AUTO_TEST_SUITE_END() // icmpv4_type_test

BOOST_AUTO_TEST_SUITE(icmpv4_code_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::icmpv4_code;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ICMPV4_CODE);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ICMPV4_CODE));
    }

BOOST_AUTO_TEST_SUITE_END() // icmpv4_code_test

BOOST_AUTO_TEST_SUITE(arp_op_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::arp_op;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ARP_OP);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ARP_OP));
    }

BOOST_AUTO_TEST_SUITE_END() // arp_op_test

BOOST_AUTO_TEST_SUITE(arp_spa_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::arp_spa;

        BOOST_TEST((std::is_same<sut::value_type, address_v4>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ARP_SPA);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ARP_SPA));
    }

BOOST_AUTO_TEST_SUITE_END() // arp_spa_test

BOOST_AUTO_TEST_SUITE(arp_tpa_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::arp_tpa;

        BOOST_TEST((std::is_same<sut::value_type, address_v4>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ARP_TPA);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ARP_TPA));
    }

BOOST_AUTO_TEST_SUITE_END() // arp_tpa_test

BOOST_AUTO_TEST_SUITE(arp_sha_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::arp_sha;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ARP_SHA);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ARP_SHA));
    }

BOOST_AUTO_TEST_SUITE_END() // arp_sha_test

BOOST_AUTO_TEST_SUITE(arp_tha_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::arp_tha;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ARP_THA);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ARP_THA));
    }

BOOST_AUTO_TEST_SUITE_END() // arp_tha_test

BOOST_AUTO_TEST_SUITE(ipv6_src_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_src;

        BOOST_TEST((std::is_same<sut::value_type, address_v6>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_SRC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_SRC));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_src_test

BOOST_AUTO_TEST_SUITE(ipv6_dst_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_dst;

        BOOST_TEST((std::is_same<sut::value_type, address_v6>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_DST);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_DST));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_dst_test

BOOST_AUTO_TEST_SUITE(ipv6_flabel_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_flabel;

        BOOST_TEST((std::is_same<sut::value_type, std::uint32_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_FLABEL);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_FLABEL));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_flabel_test

BOOST_AUTO_TEST_SUITE(icmpv6_type_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::icmpv6_type;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ICMPV6_TYPE);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ICMPV6_TYPE));
    }

BOOST_AUTO_TEST_SUITE_END() // icmpv6_type_test

BOOST_AUTO_TEST_SUITE(icmpv6_code_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::icmpv6_code;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_ICMPV6_CODE);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_ICMPV6_CODE));
    }

BOOST_AUTO_TEST_SUITE_END() // icmpv6_code_test

BOOST_AUTO_TEST_SUITE(ipv6_nd_target_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_nd_target;

        BOOST_TEST((std::is_same<sut::value_type, address_v6>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_ND_TARGET);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_ND_TARGET));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_nd_target_test

BOOST_AUTO_TEST_SUITE(ipv6_nd_sll_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_nd_sll;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_ND_SLL);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_ND_SLL));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_nd_sll_test

BOOST_AUTO_TEST_SUITE(ipv6_nd_tll_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_nd_tll;

        BOOST_TEST((std::is_same<sut::value_type, mac_address>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_ND_TLL);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_ND_TLL));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_nd_tll_test

BOOST_AUTO_TEST_SUITE(mpls_label_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::mpls_label;

        BOOST_TEST((std::is_same<sut::value_type, std::uint32_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_MPLS_LABEL);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_MPLS_LABEL));
    }

BOOST_AUTO_TEST_SUITE_END() // mpls_label_test

BOOST_AUTO_TEST_SUITE(mpls_tc_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::mpls_tc;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_MPLS_TC);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_MPLS_TC));
    }

BOOST_AUTO_TEST_SUITE_END() // mpls_tc_test

BOOST_AUTO_TEST_SUITE(mpls_bos_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::mpls_bos;

        BOOST_TEST((std::is_same<sut::value_type, std::uint8_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_MPLS_BOS);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_MPLS_BOS));
    }

BOOST_AUTO_TEST_SUITE_END() // mpls_bos_test

BOOST_AUTO_TEST_SUITE(pbb_isid_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::pbb_isid;

        BOOST_TEST((std::is_same<sut::value_type, big_uint24_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_PBB_ISID);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_PBB_ISID));
    }

BOOST_AUTO_TEST_SUITE_END() // pbb_isid_test

BOOST_AUTO_TEST_SUITE(tunnel_id_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::tunnel_id;

        BOOST_TEST((std::is_same<sut::value_type, std::uint64_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_TUNNEL_ID);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_TUNNEL_ID));
    }

BOOST_AUTO_TEST_SUITE_END() // tunnel_id_test

BOOST_AUTO_TEST_SUITE(ipv6_exthdr_test)

    BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
    {
        using sut = match::ipv6_exthdr;

        BOOST_TEST((std::is_same<sut::value_type, std::uint16_t>::value));
        BOOST_TEST(sut::oxm_class() == proto::OFPXMC_OPENFLOW_BASIC);
        BOOST_TEST(sut::oxm_field() == proto::OFPXMT_OFB_IPV6_EXTHDR);
        BOOST_TEST(sut::oxm_type() == OXM_TYPE(OXM_OF_IPV6_EXTHDR));
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_exthdr_test

BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
