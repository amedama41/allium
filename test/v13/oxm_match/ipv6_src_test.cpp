#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <stdexcept>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "../../test_utility.hpp"
#include "./oxm_match_test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;
using boost::asio::ip::address;
using boost::asio::ip::address_v6;

namespace bdata = boost::unit_test::data;

namespace {

auto operator""_ipv6(char const* const ipv6, std::size_t)
    -> address_v6
{
    return address_v6::from_string(ipv6);
}

struct no_mask_ipv6_src_fixture
{
    match::ipv6_src sut{"2001:db8::1234:0:0:9abc"_ipv6};
    std::vector<std::uint8_t> binary
        = "\x80\x00\x34\x10\x20\x01\x0d\xb8""\x00\x00\x00\x00\x12\x34\x00\x00"
          "\x00\x00\x9a\xbc"_bin;
};

struct has_mask_ipv6_src_fixture
{
    match::ipv6_src sut{
        "2001:db8::1234:0:0:9abc"_ipv6, "ffff:fdb8::ffff:1:2:ffff"_ipv6
    };
    std::vector<std::uint8_t> binary
        = "\x80\x00\x35\x20\x20\x01\x0d\xb8""\x00\x00\x00\x00\x12\x34\x00\x00"
          "\x00\x00\x9a\xbc\xff\xff\xfd\xb8""\x00\x00\x00\x00\xff\xff\x00\x01"
          "\x00\x02\xff\xff"_bin;
};

}

BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(ipv6_src_test)

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const value = "2001:db8:bd05:1d2:288a:1fc0:1:10ee"_ipv6;

        auto const sut = match::ipv6_src{value};

        BOOST_TEST(sut.oxm_length() == 16);
        BOOST_TEST(sut.oxm_header() == OXM_OF_IPV6_SRC);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_value_and_mask_test)
    {
        auto const value = "fe80::288a:1fc0:1:10ee"_ipv6;
        auto const mask = "ffff:0:ffff:0:ffff:1ff1:1111:11ff"_ipv6;

        auto const sut = match::ipv6_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_header() == OXM_OF_IPV6_SRC_W);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_zero_mask_test)
    {
        auto const value = "::"_ipv6;
        auto const mask = "::"_ipv6;

        auto const sut = match::ipv6_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_one_mask_test)
    {
        auto const value = "fd00:1234::5678"_ipv6;
        auto const mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"_ipv6;

        auto const sut = match::ipv6_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_address_type_value_test)
    {
        auto const value = address::from_string("::1");

        auto const sut = match::ipv6_src{value};

        BOOST_TEST(sut.oxm_length() == 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_address_type_value_and_mask_test)
    {
        auto const value = address::from_string("::ffff:192.168.1.10");
        auto const mask = address::from_string("::ffff:ffff:0:0");

        auto const sut = match::ipv6_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_prefix_test)
    {
        auto const value = "ff02::1"_ipv6;
        auto const prefix_length = 32;

        auto const sut = match::ipv6_src{value, prefix_length};

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == "ffff:ffff::"_ipv6);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_zero_prefix_test)
    {
        auto const value = "::"_ipv6;
        auto const prefix_length = 0;

        auto const sut = match::ipv6_src{value, prefix_length};

        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == "::"_ipv6);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_full_prefix_test)
    {
        auto const value = "2001:db8::9abc"_ipv6;
        auto const prefix_length = 128;

        auto const sut = match::ipv6_src{value, prefix_length};

        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask()
                == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"_ipv6);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(no_mask_equality_test)
    {
        auto const sut = match::ipv6_src{"ff01::1"_ipv6};
        auto const same_value = match::ipv6_src{"ff01::1"_ipv6};
        auto const diff_value = match::ipv6_src{"ff02::1"_ipv6};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_AUTO_TEST_CASE(has_mask_equality_test)
    {
        auto const sut = match::ipv6_src{"ff01::1"_ipv6, 8};
        auto const same_value_and_same_mask
            = match::ipv6_src{"ff01::1"_ipv6, 8};
        auto const same_value_and_diff_mask
            = match::ipv6_src{"ff01::1"_ipv6, 16};
        auto const diff_value_and_same_mask
            = match::ipv6_src{"ff02::1"_ipv6, 8};
        auto const diff_value_and_diff_mask
            = match::ipv6_src{"ff02::1"_ipv6, 16};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value_and_same_mask));
        BOOST_TEST((sut != same_value_and_diff_mask));
        BOOST_TEST((sut != diff_value_and_same_mask));
        BOOST_TEST((sut != diff_value_and_diff_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_and_has_mask_equality_test)
    {
        auto const sut = match::ipv6_src{"fd00::1:2"_ipv6};
        auto const same_value_and_mask = match::ipv6_src{"fd00::1:2"_ipv6, 24};
        auto const diff_value_and_mask = match::ipv6_src{"fc00::1:2"_ipv6, 24};

        BOOST_TEST((sut != same_value_and_mask));
        BOOST_TEST((sut != diff_value_and_mask));
    }

    BOOST_AUTO_TEST_CASE(exact_and_wildcard_equality_test)
    {
        auto const sut = match::ipv6_src{"::1"_ipv6};
        auto const same_value_and_all_zero_mask
            = match::ipv6_src{"::1"_ipv6, 0};
        auto const same_value_and_all_one_mask
            = match::ipv6_src{"::1"_ipv6, 128};
        auto const diff_value_and_all_zero_mask
            = match::ipv6_src{"::"_ipv6, 0};
        auto const diff_value_and_all_one_mask
            = match::ipv6_src{"::"_ipv6, 128};

        BOOST_TEST((sut != same_value_and_all_zero_mask));
        BOOST_TEST((sut == same_value_and_all_one_mask));
        BOOST_TEST((sut != diff_value_and_all_zero_mask));
        BOOST_TEST((sut != diff_value_and_all_one_mask));
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<address_v6>{
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
        auto const sut = match::ipv6_src::create(value);

        BOOST_TEST(sut.oxm_length() == 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              has_mask_create_success_test
            , bdata::make(std::vector<address_v6>{
                  "0000:0000:0000:0000:0000:0000:0000:0000"_ipv6
                , "2001:0000:0000:0000:0000:0000:0000:0000"_ipv6
                , "2001:0db8:0000:0000:0000:0000:0000:0000"_ipv6
                , "2001:0db8:bd05:0000:0000:0000:0000:0000"_ipv6
                , "2001:0db8:bd05:01d2:0000:0000:0000:0000"_ipv6
                , "2001:0db8:bd05:01d2:288a:0000:0000:0000"_ipv6
                , "2001:0db8:bd05:01d2:288a:1fc0:0000:0000"_ipv6
                , "2001:0db8:bd05:01d2:288a:1fc0:0001:0000"_ipv6
                , "2001:0db8:bd05:01d2:288a:1fc0:0001:10ee"_ipv6
              })
            ^ bdata::make(std::vector<address_v6>{
                  "0000:0000:0000:0000:0000:0000:0000:0001"_ipv6
                , "2003:0000:0000:0000:0000:0000:0000:0001"_ipv6
                , "2003:0db9:0000:0000:0000:0000:0000:0001"_ipv6
                , "2003:0db9:bd07:0000:0000:0000:0000:0001"_ipv6
                , "2003:0db9:bd07:01d3:0000:0000:0000:0001"_ipv6
                , "2003:0db9:bd07:01d3:288b:0000:0000:0001"_ipv6
                , "2003:0db9:bd07:01d3:288b:1fc1:0000:0001"_ipv6
                , "2003:0db9:bd07:01d3:288b:1fc1:0003:0001"_ipv6
                , "2003:0db9:bd07:01d3:288b:1fc1:0003:10ef"_ipv6
              })
            , value, mask)
    {
        auto const sut = match::ipv6_src::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * 16);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length() == sizeof(std::uint32_t) + 2 * 16);
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                  match::ipv6_src::create("0202::0304"_ipv6, "1020::3040"_ipv6)
                , std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_encode_test, no_mask_ipv6_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_decode_test, no_mask_ipv6_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::ipv6_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_encode_test, has_mask_ipv6_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_decode_test, has_mask_ipv6_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::ipv6_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_src_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
