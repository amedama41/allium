#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <stdexcept>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "../../test_utility.hpp"
#include "../oxm_headers.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;

namespace bdata = boost::unit_test::data;

namespace {

struct no_mask_ipv4_src_fixture
{
    match::ipv4_src sut{"192.168.12.31"_ipv4};
    std::vector<std::uint8_t> binary = "\x80\x00\x16\x04\xc0\xa8\x0c\x1f"_bin;
};

struct has_mask_ipv4_src_fixture
{
    match::ipv4_src sut{"192.168.12.31"_ipv4, "255.168.255.31"_ipv4};
    std::vector<std::uint8_t> binary
        = "\x80\x00\x17\x08\xc0\xa8\x0c\x1f""\xff\xa8\xff\x1f"_bin;
};

}

BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(ipv4_src_test)

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const value = "127.0.0.1"_ipv4;

        auto const sut = match::ipv4_src{value};

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_IPV4_SRC);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_value_and_mask_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const mask = "255.0.0.255"_ipv4;

        auto const sut = match::ipv4_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_IPV4_SRC_W);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_zero_mask_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const mask = "0.0.0.0"_ipv4;

        auto const sut = match::ipv4_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_one_mask_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const mask = "255.255.255.255"_ipv4;

        auto const sut = match::ipv4_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_address_type_value_test)
    {
        auto const value = address::from_string("127.0.0.1");

        auto const sut = match::ipv4_src{value};

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_address_type_value_and_mask_test)
    {
        auto const value = address::from_string("127.0.0.1");
        auto const mask = address::from_string("255.0.0.1");

        auto const sut = match::ipv4_src{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_prefix_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const prefix_length = 16;

        auto const sut = match::ipv4_src{value, prefix_length};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == "255.255.0.0"_ipv4);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_zero_prefix_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const prefix_length = 0;

        auto const sut = match::ipv4_src{value, prefix_length};

        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == "0.0.0.0"_ipv4);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_full_prefix_test)
    {
        auto const value = "127.0.0.1"_ipv4;
        auto const prefix_length = 32;

        auto const sut = match::ipv4_src{value, prefix_length};

        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == "255.255.255.255"_ipv4);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(no_mask_equality_test)
    {
        auto const sut = match::ipv4_src{address_v4{0x01020304}};
        auto const same_value = match::ipv4_src{address_v4{0x01020304}};
        auto const diff_value = match::ipv4_src{address_v4{0x11121314}};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_AUTO_TEST_CASE(has_mask_equality_test)
    {
        auto const sut = match::ipv4_src{address_v4{0x01020304}, 16};
        auto const same_value_and_same_mask
            = match::ipv4_src{address_v4{0x01020304}, 16};
        auto const same_value_and_diff_mask
            = match::ipv4_src{address_v4{0x01020304}, 24};
        auto const diff_value_and_same_mask
            = match::ipv4_src{address_v4{0x11121314}, 16};
        auto const diff_value_and_diff_mask
            = match::ipv4_src{address_v4{0x11121314}, 24};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value_and_same_mask));
        BOOST_TEST((sut != same_value_and_diff_mask));
        BOOST_TEST((sut != diff_value_and_same_mask));
        BOOST_TEST((sut != diff_value_and_diff_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_and_has_mask_equality_test)
    {
        auto const sut = match::ipv4_src{address_v4{0x01020304}};
        auto const same_value_and_mask
            = match::ipv4_src{address_v4{0x01020304}, 24};
        auto const diff_value_and_mask
            = match::ipv4_src{address_v4{0x01f2f3f4}, 24};

        BOOST_TEST((sut != same_value_and_mask));
        BOOST_TEST((sut != diff_value_and_mask));
    }

    BOOST_AUTO_TEST_CASE(exact_and_wildcard_equality_test)
    {
        auto const sut = match::ipv4_src{address_v4{0x01020304}};
        auto const same_value_and_all_zero_mask
            = match::ipv4_src{address_v4{0x01020304}, 0};
        auto const same_value_and_all_one_mask
            = match::ipv4_src{address_v4{0x01020304}, 32};
        auto const diff_value_and_all_zero_mask
            = match::ipv4_src{address_v4{0xf1f2f3f4}, 0};
        auto const diff_value_and_all_one_mask
            = match::ipv4_src{address_v4{0xf1f2f3f4}, 32};

        BOOST_TEST((sut != same_value_and_all_zero_mask));
        BOOST_TEST((sut == same_value_and_all_one_mask));
        BOOST_TEST((sut != diff_value_and_all_zero_mask));
        BOOST_TEST((sut != diff_value_and_all_one_mask));
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<address_v4>{
                  address_v4{0x00000000}, address_v4{0xff000000}
                , address_v4{0xffff0000}, address_v4{0xffffff00}
                , address_v4{0xffffffff}
              })
            , value)
    {
        auto const sut = match::ipv4_src::create(value);

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              has_mask_create_success_test
            , bdata::make(std::vector<address_v4>{
                  address_v4{0x00000000}, address_v4{0xc0000000}
                , address_v4{0xc0a80000}, address_v4{0xc0a80a00}
                , address_v4{0xc0a80a1f}
              })
            ^ bdata::make(std::vector<address_v4>{
                  address_v4{0x00000001}, address_v4{0xff000000}
                , address_v4{0xffff0000}, address_v4{0xffffff00}
                , address_v4{0xffffff1f}
              })
            , value, mask)
    {
        auto const sut = match::ipv4_src::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                  match::ipv4_src::create(
                    address_v4{0x01020304}, address_v4{0x10203040})
                , std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_decode_test, no_mask_ipv4_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::ipv4_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_encode_test, no_mask_ipv4_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_encode_test, has_mask_ipv4_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_decode_test, has_mask_ipv4_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::ipv4_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

BOOST_AUTO_TEST_SUITE_END() // ipv4_src_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
