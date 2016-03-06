#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/set_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;
namespace actions = v13::actions;
namespace v13_detail = v13::v13_detail;

using protocol = v13::protocol;

namespace bdata = boost::unit_test::data;

namespace {

struct set_eth_dst_fixture
{
    actions::set_eth_dst sut{"\x01\x02\x03\x04\x05\x06"_mac};
    std::vector<std::uint8_t> binary
        = "\x00\x19\x00\x10\x80\x00\x06\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          ""_bin;
};

struct set_vlan_vid_fixture
{
    actions::set_vlan_vid sut{protocol::OFPVID_PRESENT | 0x0123};
    std::vector<std::uint8_t> binary
        = "\x00\x19\x00\x10\x80\x00\x0c\x02""\x11\x23\x00\x00\x00\x00\x00\x00"
          ""_bin;
};

struct set_ipv4_src_fixture
{
    actions::set_ipv4_src sut{"192.168.10.3"_ipv4};
    std::vector<std::uint8_t> binary
        = "\x00\x19\x00\x10\x80\x00\x16\x04""\xc0\xa8\x0a\x03\x00\x00\x00\x00"
          ""_bin;
};

struct set_ipv6_src_fixture
{
    actions::set_ipv6_src sut{"2001:db8::1234:0:0:9abc"_ipv6};
    std::vector<std::uint8_t> binary
        = "\x00\x19\x00\x18\x80\x00\x34\x10""\x20\x01\x0d\xb8\x00\x00\x00\x00"
          "\x12\x34\x00\x00\x00\x00\x9a\xbc"_bin;
};

}

BOOST_AUTO_TEST_SUITE(action_test)
BOOST_AUTO_TEST_SUITE(set_field_test)

BOOST_AUTO_TEST_SUITE(set_eth_dst_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::set_eth_dst;

        BOOST_TEST(sut::type() == protocol::OFPAT_SET_FIELD);
        BOOST_TEST(sut::oxm_type() == match::eth_dst::oxm_type());
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const field = match::eth_dst{"\x11\x12\x13\x14\x15\x16"_mac};

        auto const sut = actions::set_eth_dst{field.oxm_value()};

        BOOST_TEST(sut.length() == ((4 + field.length()) + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const field = match::eth_dst{"\xf1\xf2\xf3\xf4\xf5\xf6"_mac};

        auto const sut = actions::set_eth_dst::create(field.oxm_value());

        BOOST_TEST(sut.length() == ((4 + field.length()) + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut
            = actions::set_eth_dst{"\x01\x02\x03\x04\x05\x06"_mac};
        auto const same_value
            = actions::set_eth_dst{"\x01\x02\x03\x04\x05\x06"_mac};
        auto const diff_value
            = actions::set_eth_dst{"\x11\x12\x13\x14\x15\x16"_mac};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_eth_dst_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_eth_dst_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_field = actions::set_eth_dst::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_field == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_eth_dst_test

BOOST_AUTO_TEST_SUITE(set_vlan_vid_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::set_vlan_vid;

        BOOST_TEST(sut::type() == protocol::OFPAT_SET_FIELD);
        BOOST_TEST(sut::oxm_type() == match::vlan_vid::oxm_type());
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const field = match::vlan_vid{protocol::OFPVID_PRESENT | 0x0001};

        auto const sut = actions::set_vlan_vid{field.oxm_value()};

        BOOST_TEST(sut.length() == ((4 + field.length()) + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint16_t>{
                  0x0001 | protocol::OFPVID_PRESENT
                , 0x0fff | protocol::OFPVID_PRESENT
                , protocol::OFPVID_NONE
              })
            , value)
    {
        auto const field = match::vlan_vid{value};

        auto const sut = actions::set_vlan_vid::create(field.oxm_value());

        BOOST_TEST(sut.length() == ((4 + field.length()) + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(std::vector<std::uint16_t>{
                0x0fff, protocol::OFPVID_PRESENT | 0x2000, 0xffff
              })
            , value)
    {
        auto const field = match::vlan_vid{value};

        BOOST_CHECK_THROW(
                  actions::set_vlan_vid::create(field.oxm_value())
                , std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut
            = actions::set_vlan_vid{protocol::OFPVID_PRESENT | 0x0000};
        auto const same_value
            = actions::set_vlan_vid{protocol::OFPVID_PRESENT | 0x0000};
        auto const diff_value
            = actions::set_vlan_vid{protocol::OFPVID_PRESENT | 0x0001};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_vlan_vid_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_vlan_vid_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_field = actions::set_vlan_vid::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_field == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_vlan_vid_test

BOOST_AUTO_TEST_SUITE(set_ipv4_src_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::set_ipv4_src;

        BOOST_TEST(sut::type() == protocol::OFPAT_SET_FIELD);
        BOOST_TEST(sut::oxm_type() == match::ipv4_src::oxm_type());
    }

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const field = match::ipv4_src{"172.16.1.2"_ipv4};

        auto const sut = actions::set_ipv4_src{field.oxm_value()};

        BOOST_TEST(sut.length() == (4 + field.length() + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_DATA_TEST_CASE(
              create_test
            , bdata::make(std::vector<boost::asio::ip::address_v4>{
                  "0.0.0.0"_ipv4, "255.0.0.0"_ipv4, "255.255.0.0"_ipv4
                , "255.255.255.0"_ipv4, "255.255.255.255"_ipv4
              })
            , value)
    {
        auto const field = match::ipv4_src{value};

        auto const sut = actions::set_ipv4_src{field.oxm_value()};

        BOOST_TEST(sut.length() == (4 + field.length() + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = actions::set_ipv4_src{"0.0.0.0"_ipv4};
        auto const same_value = actions::set_ipv4_src{"0.0.0.0"_ipv4};
        auto const diff_value = actions::set_ipv4_src{"0.0.0.1"_ipv4};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_ipv4_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_ipv4_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_field = actions::set_ipv4_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_field == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_ipv6_src_test

BOOST_AUTO_TEST_SUITE(set_ipv6_src_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::set_ipv6_src;

        BOOST_TEST(sut::type() == protocol::OFPAT_SET_FIELD);
        BOOST_TEST(sut::oxm_type() == match::ipv6_src::oxm_type());
    }

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const field
            = match::ipv6_src{"2001:db8:bd05:1d2:288a:1fc0:1:10ee"_ipv6};

        auto const sut = actions::set_ipv6_src{field.oxm_value()};

        BOOST_TEST(sut.length() == (4 + field.length() + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_DATA_TEST_CASE(
              create_test
            , bdata::make(std::vector<boost::asio::ip::address_v6>{
                  "0000:0000:0000:0000:0000:0000:0000:0000"_ipv6
                , "ffff:0000:0000:0000:0000:0000:0000:0000"_ipv6
                , "ffff:ffff:0000:0000:0000:0000:0000:0000"_ipv6
                , "ffff:ffff:ffff:0000:0000:0000:0000:0000"_ipv6
                , "ffff:ffff:ffff:ffff:0000:0000:0000:0000"_ipv6
                , "ffff:ffff:ffff:ffff:ffff:0000:0000:0000"_ipv6
                , "ffff:ffff:ffff:ffff:ffff:ffff:0000:0000"_ipv6
                , "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0000"_ipv6
                , "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"_ipv6
              })
            , value)
    {
        auto const field = match::ipv6_src{value};

        auto const sut = actions::set_ipv6_src{field.oxm_value()};

        BOOST_TEST(sut.length() == (4 + field.length() + 7) / 8 * 8);
        BOOST_TEST(sut.value() == field.oxm_value());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = actions::set_ipv6_src{"::"_ipv6};
        auto const same_value = actions::set_ipv6_src{"::"_ipv6};
        auto const diff_value = actions::set_ipv6_src{"::1"_ipv6};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_ipv6_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_ipv6_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_field = actions::set_ipv6_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_field == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_ipv6_src_test

BOOST_AUTO_TEST_SUITE_END() // set_field_test
BOOST_AUTO_TEST_SUITE_END() // action_test
