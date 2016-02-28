#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/asio/ip/address_v4.hpp>

namespace of = canard::network::openflow;
namespace v10 = of::v10;
namespace detail = v10::v10_detail;
namespace match = v10::match;

using proto = v10::protocol;

namespace {

    struct match_field_fixture
    {
        using mac_address = canard::mac_address;
        using address_v4 = boost::asio::ip::address_v4;

        match::in_port const in_port{proto::OFPP_MAX};
        match::eth_src const eth_src{
            mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        match::eth_dst const eth_dst{
            mac_address{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}
        };
        match::vlan_vid const vlan_vid{0x0fff};
        match::vlan_pcp const vlan_pcp{0x07};
        match::eth_type const eth_type{0x8100};
        match::ip_dscp const ip_dscp{0x3f};
        match::ip_proto const ip_proto{6};
        match::ipv4_src const ipv4_src{
            address_v4::from_string("192.168.1.0"), 24
        };
        match::ipv4_dst const ipv4_dst{
            address_v4::from_string("127.0.0.1"), 32
        };
        match::tcp_src const tcp_src{6633};
        match::tcp_dst const tcp_dst{6653};
    };

    struct match_set_fixture : match_field_fixture
    {
        v10::match_set sut{
              in_port, eth_src, eth_dst, vlan_vid, vlan_pcp, eth_type
            , ip_dscp, ip_proto, ipv4_src, ipv4_dst, tcp_src, tcp_dst
        };
    };
}

BOOST_AUTO_TEST_SUITE(common_types_test)
BOOST_AUTO_TEST_SUITE(match_set_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v10::match_set{};

        BOOST_TEST(sut.length() == sizeof(detail::ofp_match));
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.wildcards() == proto::OFPFW_ALL);
        BOOST_TEST((sut.get<match::in_port>() == boost::none));
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_field_test)
    {
        auto const in_port = match::in_port{1};

        auto const sut = v10::match_set{in_port};

        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.ofp_match().in_port == in_port.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_IN_PORT);
        BOOST_TEST((sut.get<match::in_port>() == in_port));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_different_fields_test)
    {
        auto const in_port = match::in_port{1};
        auto const eth_src = match::eth_src{
            canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        auto const vlan_vid = match::vlan_vid{0x0fff};
        auto const ipv4_dst = match::ipv4_dst{
            boost::asio::ip::address_v4::from_string("172.16.0.0"), 16
        };
        auto const icmpv4_type = match::icmpv4_type{8};

        auto const sut = v10::match_set{
            in_port, eth_src, vlan_vid, ipv4_dst, icmpv4_type
        };

        BOOST_TEST(!sut.empty());

        BOOST_TEST(sut.ofp_match().in_port == in_port.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_IN_PORT);
        BOOST_TEST((sut.get<match::in_port>() == in_port));

        BOOST_TEST(canard::mac_address{sut.ofp_match().dl_src} == eth_src.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_DL_SRC);
        BOOST_TEST((sut.get<match::eth_src>() == eth_src));

        BOOST_TEST(sut.ofp_match().dl_vlan == vlan_vid.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_DL_VLAN);
        BOOST_TEST((sut.get<match::vlan_vid>() == vlan_vid));

        BOOST_TEST(sut.ofp_match().nw_dst == ipv4_dst.value().to_ulong());
        BOOST_TEST((sut.get<match::ipv4_dst>() == ipv4_dst));
        BOOST_TEST(((sut.wildcards() & proto::OFPFW_NW_DST_MASK)
                    >> proto::OFPFW_NW_DST_SHIFT)
                == ipv4_dst.wildcard_bit_count());

        BOOST_TEST(sut.ofp_match().tp_src == icmpv4_type.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_TP_SRC);
        BOOST_TEST((sut.get<match::icmpv4_type>() == icmpv4_type));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_same_fields_test)
    {
        using boost::asio::ip::address_v4;
        auto const ipv4_src1
            = match::ipv4_src{address_v4::from_string("172.16.0.0"), 16};
        auto const ipv4_src2
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const tcp_dst1 = match::tcp_dst{11};
        auto const tcp_dst2 = match::tcp_dst{21};

        auto const sut = v10::match_set{
            ipv4_src1, tcp_dst1, tcp_dst2, ipv4_src2
        };

        BOOST_TEST(sut.ofp_match().nw_src == ipv4_src2.value().to_ulong());
        BOOST_TEST(((sut.wildcards() & proto::OFPFW_NW_SRC_MASK)
                    >> proto::OFPFW_NW_SRC_SHIFT)
                == ipv4_src2.wildcard_bit_count());
        BOOST_TEST((sut.get<match::ipv4_src>() == ipv4_src2));

        BOOST_TEST(sut.ofp_match().tp_dst == tcp_dst2.value());
        BOOST_TEST(~sut.wildcards() & proto::OFPFW_TP_DST);
        BOOST_TEST((sut.get<match::tcp_dst>() == tcp_dst2));
    }

    BOOST_AUTO_TEST_CASE(construct_from_dscp_field_test)
    {
        auto const ip_dscp = match::ip_dscp{0x11};

        auto const sut = v10::match_set{ip_dscp};

        BOOST_TEST(~sut.wildcards() & proto::OFPFW_NW_TOS);
        BOOST_TEST(sut.ofp_match().nw_tos == (ip_dscp.value() << 2));
        BOOST_TEST((sut.get<match::ip_dscp>() == ip_dscp));
    }

    BOOST_AUTO_TEST_CASE(construct_from_invalid_fields_test)
    {
        auto const in_port = match::in_port{1};
        auto const vlan_vid = match::vlan_vid{0xffff};
        auto const vlan_pcp = match::vlan_pcp{0xff};

        auto const sut = v10::match_set{
            in_port, vlan_vid, vlan_pcp
        };

        BOOST_TEST((sut.get<match::in_port>() == in_port));
        BOOST_TEST((sut.get<match::vlan_vid>() == vlan_vid));
        BOOST_TEST((sut.get<match::vlan_pcp>() == vlan_pcp));
    }

    BOOST_AUTO_TEST_CASE(construct_from_ofp_match_test)
    {
        auto ofp_match = detail::ofp_match{};
        ofp_match.wildcards = proto::OFPFW_ALL;
        ofp_match.wildcards &= ~proto::OFPFW_IN_PORT;
        ofp_match.in_port = proto::OFPP_ANY;
        ofp_match.wildcards &= ~proto::OFPFW_DL_VLAN;
        ofp_match.dl_vlan = 0xfffe;
        ofp_match.wildcards &= ~proto::OFPFW_DL_VLAN_PCP;
        ofp_match.dl_vlan_pcp = 0xff;
        ofp_match.wildcards &= ~proto::OFPFW_NW_TOS;
        ofp_match.nw_tos = 0xff;

        auto const sut = v10::match_set{ofp_match};

        BOOST_TEST((sut.get<match::in_port>() != boost::none));
        BOOST_TEST(sut.get<match::in_port>()->value() == ofp_match.in_port);
        BOOST_TEST((sut.get<match::vlan_vid>() != boost::none));
        BOOST_TEST(sut.get<match::vlan_vid>()->value() == ofp_match.dl_vlan);
        BOOST_TEST((sut.get<match::vlan_pcp>() != boost::none));
        BOOST_TEST(sut.get<match::vlan_pcp>()->value() == ofp_match.dl_vlan_pcp);
        BOOST_TEST((sut.get<match::ip_dscp>() != boost::none));
        BOOST_TEST(sut.get<match::ip_dscp>()->value() == (ofp_match.nw_tos >> 2));
        BOOST_TEST((sut.get<match::ipv4_src>() == boost::none));
        BOOST_TEST((sut.get<match::ipv4_dst>() == boost::none));
    }

    BOOST_AUTO_TEST_CASE(set_field_test)
    {
        auto const eth_type = match::eth_type{0x0800};
        auto sut = v10::match_set{};

        sut.set(eth_type);

        BOOST_TEST(~sut.wildcards() & proto::OFPFW_DL_TYPE);
        BOOST_TEST(sut.ofp_match().dl_type == eth_type.value());
        BOOST_TEST((sut.get<match::eth_type>() == eth_type));
    }

    BOOST_FIXTURE_TEST_CASE(set_existed_field_test, match_set_fixture)
    {
        auto const in_port = match::in_port{1};

        sut.set(in_port);

        BOOST_TEST(~sut.wildcards() & proto::OFPFW_IN_PORT);
        BOOST_TEST(sut.ofp_match().in_port == in_port.value());
        BOOST_TEST((sut.get<match::in_port>() == in_port));
    }

    BOOST_FIXTURE_TEST_CASE(set_all_mask_ip_addr_field_test, match_set_fixture)
    {
        auto const ipv4_src
            = match::ipv4_src{address_v4::from_string("0.0.0.0"), 0};

        sut.set(ipv4_src);

        BOOST_TEST(sut.wildcards() & proto::OFPFW_NW_SRC_ALL);
        BOOST_TEST((sut.get<match::ipv4_src>() == boost::none));
    }

    BOOST_FIXTURE_TEST_CASE(erase_test, match_set_fixture)
    {
        sut.erase<match::in_port>();

        BOOST_TEST(sut.wildcards() & proto::OFPFW_IN_PORT);
        BOOST_TEST(sut.ofp_match().in_port == 0);
        BOOST_TEST((sut.get<match::in_port>() == boost::none));
    }

    BOOST_FIXTURE_TEST_CASE(erase_ip_addr_test, match_set_fixture)
    {
        sut.erase<match::ipv4_src>();

        BOOST_TEST(sut.wildcards() & proto::OFPFW_NW_SRC_ALL);
        BOOST_TEST(sut.ofp_match().nw_src == 0);
        BOOST_TEST((sut.get<match::ipv4_src>() == boost::none));
    }

    BOOST_FIXTURE_TEST_CASE(equality_test1, match_field_fixture)
    {
        auto const sut1 = v10::match_set{
              in_port, eth_src, eth_dst, vlan_vid, vlan_pcp, eth_type
            , ip_dscp, ip_proto, ipv4_src, ipv4_dst, tcp_src, tcp_dst
        };
        auto sut2 = v10::match_set{};
        sut2.set(in_port);
        sut2.set(eth_src);
        sut2.set(eth_dst);
        sut2.set(vlan_vid);
        sut2.set(vlan_pcp);
        sut2.set(eth_type);
        sut2.set(ip_dscp);
        sut2.set(ip_proto);
        sut2.set(ipv4_src);
        sut2.set(ipv4_dst);
        sut2.set(tcp_src);
        sut2.set(tcp_dst);
        auto const sut3 = v10::match_set{
              eth_src, eth_dst, vlan_vid, vlan_pcp, eth_type
            , ip_dscp, ip_proto, ipv4_src, ipv4_dst, tcp_src, tcp_dst
        };
        auto sut4 = v10::match_set{};
        sut4.set(eth_src);
        sut4.set(eth_dst);
        sut4.set(vlan_vid);
        sut4.set(vlan_pcp);
        sut4.set(eth_type);
        sut4.set(ip_dscp);
        sut4.set(ip_proto);
        sut4.set(ipv4_src);
        sut4.set(ipv4_dst);
        sut4.set(tcp_src);
        sut4.set(tcp_dst);

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
        BOOST_TEST((sut1 != sut4));
        BOOST_TEST((sut3 == sut4));
    }

    BOOST_AUTO_TEST_CASE(equality_test2)
    {
        auto const sut1 = v10::match_set{};
        auto const sut2 = v10::match_set{};
        auto const sut3 = v10::match_set{match::ip_proto{17}};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_FIXTURE_TEST_CASE(equality_test3, match_field_fixture)
    {
        auto const ipv4_src1
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const ipv4_src2
            = match::ipv4_src{address_v4::from_string("192.168.1.1"), 24};
        auto const ipv4_src3
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 32};
        auto const sut1 = v10::match_set{
              in_port, eth_src, vlan_vid, vlan_pcp, ip_dscp, ipv4_src1, tcp_src
        };
        auto const sut2 = v10::match_set{
              in_port, eth_src, vlan_vid, vlan_pcp, ip_dscp, ipv4_src2, tcp_src
        };
        auto const sut3 = v10::match_set{
              in_port, eth_src, vlan_vid, vlan_pcp, ip_dscp, ipv4_src3, tcp_src
        };

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

BOOST_AUTO_TEST_SUITE_END() // match_set_test
BOOST_AUTO_TEST_SUITE_END() // common_types_test
