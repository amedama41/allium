#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v10/match_fields.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/monomorphic/generators/xrange.hpp>

#include <cstdint>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>

namespace of = canard::network::openflow;
namespace v10 = of::v10;
namespace detail = v10::v10_detail;
namespace match = v10::match;

using proto = v10::protocol;

namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(match_fields_test)

BOOST_AUTO_TEST_SUITE(in_port_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port_no = std::uint16_t{1};

        auto const sut = match::in_port{port_no};

        BOOST_TEST(sut.value() == port_no);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = match::in_port{1};
        auto const sut2 = match::in_port{1};
        auto const sut3 = match::in_port{2};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint16_t>{
                1, proto::OFPP_MAX, proto::OFPP_CONTROLLER, proto::OFPP_LOCAL
              })
            , port_no)
    {
        auto const sut = match::in_port::create(port_no);

        BOOST_TEST(sut.value() == port_no);
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(std::vector<std::uint16_t>{
                  0
                , proto::OFPP_IN_PORT, proto::OFPP_TABLE, proto::OFPP_NORMAL
                , proto::OFPP_FLOOD, proto::OFPP_ALL, proto::OFPP_NONE
              })
            , port_no)
    {
        BOOST_CHECK_THROW(match::in_port::create(port_no), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // in_port_test

BOOST_AUTO_TEST_SUITE(vlan_vid_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const vid = std::uint16_t{0x0fff};

        auto const sut = match::vlan_vid{vid};

        BOOST_TEST(sut.value() == vid);
    }

    BOOST_AUTO_TEST_CASE(construct_from_over_vid_test)
    {
        auto const vid = std::uint16_t{0xffff};

        auto const sut = match::vlan_vid{vid};

        BOOST_TEST(sut.value() == 0xffff);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint16_t>{
                0x0000, 0x1000 / 2, 0x0fff, proto::OFP_VLAN_NONE
              })
            , vid)
    {
        auto const sut = match::vlan_vid::create(vid);

        BOOST_TEST(sut.value() == vid);
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(std::vector<std::uint16_t>{
                0x1000, (0x1000 + 0xffff) / 2, 0xfffe
              })
            , vid)
    {
        BOOST_CHECK_THROW(match::vlan_vid::create(vid), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // vlan_vid_test

BOOST_AUTO_TEST_SUITE(vlan_pcp_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const pcp = std::uint8_t{0x01};

        auto const sut = match::vlan_pcp{pcp};

        BOOST_TEST(sut.value() == pcp);
    }

    BOOST_DATA_TEST_CASE(create_success_test, bdata::xrange(0x00, 0x08), pcp)
    {
        auto const sut = match::vlan_pcp::create(pcp);

        BOOST_TEST(sut.value() == pcp);
    }

    BOOST_DATA_TEST_CASE(create_failure_test, bdata::xrange(0x08, 0xff), pcp)
    {
        BOOST_CHECK_THROW(match::vlan_pcp::create(pcp), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // vlan_pcp_test

BOOST_AUTO_TEST_SUITE(ip_dscp_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const dscp = std::uint8_t{0x01};

        auto const sut = match::ip_dscp{dscp};

        BOOST_TEST(sut.value() == dscp);
    }

    BOOST_DATA_TEST_CASE(create_success_test, bdata::xrange(0x00, 0x40), dscp)
    {
        auto const sut = match::ip_dscp::create(dscp);

        BOOST_TEST(sut.value() == dscp);
    }

    BOOST_DATA_TEST_CASE(create_failure_test, bdata::xrange(0x40, 0xff), dscp)
    {
        BOOST_CHECK_THROW(match::ip_dscp::create(dscp), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // ip_dscp_test

BOOST_AUTO_TEST_SUITE(eth_src_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const addr
            = canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};

        auto const sut = match::eth_src{addr};

        BOOST_TEST(sut.value() == addr);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = match::eth_src{
            canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        auto const sut2 = match::eth_src{
            canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        auto const sut3 = match::eth_src{
            canard::mac_address{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}
        };

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const addr
            = canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};

        auto const sut = match::eth_src::create(addr);

        BOOST_TEST(sut.value() == addr);
    }

BOOST_AUTO_TEST_SUITE_END() // eth_src_test

BOOST_AUTO_TEST_SUITE(ipv4_src_test)

    BOOST_AUTO_TEST_CASE(construct_from_ipaddress_test)
    {
        auto const addr = boost::asio::ip::address::from_string("127.0.0.1");

        auto const sut = match::ipv4_src{addr};

        BOOST_TEST(sut.value() == addr);
        BOOST_TEST(sut.prefix_length() == 32);
        BOOST_TEST(sut.wildcard_bit_count() == 0);
    }

    BOOST_AUTO_TEST_CASE(construct_from_ipaddress_with_prefix_length_test)
    {
        auto const addr = boost::asio::ip::address::from_string("127.0.0.1");
        auto const prefix_length = 8;

        auto const sut = match::ipv4_src{addr, prefix_length};

        BOOST_TEST(sut.value() == addr);
        BOOST_TEST(sut.prefix_length() == prefix_length);
        BOOST_TEST(sut.wildcard_bit_count() == 32 - prefix_length);
    }

    BOOST_AUTO_TEST_CASE(construct_from_ipv4address_test)
    {
        auto const addr = boost::asio::ip::address_v4::from_string("127.0.0.1");

        auto const sut = match::ipv4_src{addr};

        BOOST_TEST(sut.value() == addr);
        BOOST_TEST(sut.prefix_length() == 32);
        BOOST_TEST(sut.wildcard_bit_count() == 0);
    }

    BOOST_AUTO_TEST_CASE(construct_from_ipv4address_with_prefix_length_test)
    {
        auto const addr = boost::asio::ip::address_v4::from_string("127.0.0.1");
        auto const prefix_length = 24;

        auto const sut = match::ipv4_src{addr, prefix_length};

        BOOST_TEST(sut.value() == addr);
        BOOST_TEST(sut.prefix_length() == prefix_length);
        BOOST_TEST(sut.wildcard_bit_count() == 32 - prefix_length);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        using boost::asio::ip::address;
        auto const sut1
            = match::ipv4_src{address::from_string("192.168.1.0"), 24};
        auto const sut2
            = match::ipv4_src{address::from_string("192.168.1.0"), 24};
        auto const sut3
            = match::ipv4_src{address::from_string("192.168.1.1"), 24};
        auto const sut4
            = match::ipv4_src{address::from_string("192.168.1.0"), 16};
        auto const sut5
            = match::ipv4_src{address::from_string("192.168.1.1"), 16};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 == sut3));
        BOOST_TEST((sut1 != sut4));
        BOOST_TEST((sut1 != sut5));
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint8_t>{0, 8, 16, 24, 32})
            , prefix_length)
    {
        auto const addr = boost::asio::ip::address::from_string("192.168.2.1");

        auto const sut = match::ipv4_src::create(addr, prefix_length);

        BOOST_TEST(sut.value() == addr);
        BOOST_TEST(sut.prefix_length() == prefix_length);
        BOOST_TEST(sut.wildcard_bit_count() == 32 - prefix_length);
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(std::vector<std::uint8_t>{33, 64, 128, 255})
            , prefix_length)
    {
        auto const addr = boost::asio::ip::address::from_string("192.168.2.1");

        BOOST_CHECK_THROW(
                  match::ipv4_src::create(addr, prefix_length)
                , std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // ipv4_src_test

BOOST_AUTO_TEST_SUITE_END() // match_fields_test
