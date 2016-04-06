#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <stdexcept>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "../../test_utility.hpp"
#include "../oxm_headers.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;

namespace bdata = boost::unit_test::data;

namespace {

struct no_mask_in_port_fixture
{
    match::in_port sut{0x12345678};
    std::vector<std::uint8_t> binary = "\x80\x00\x00\x04\x12\x34\x56\x78"_bin;
};

struct has_mask_in_port_fixture
{
    match::in_port sut{0x12345678, 0x1f3f5f7f};
    std::vector<std::uint8_t> binary
        = "\x80\x00\x01\x08\x12\x34\x56\x78""\x1f\x3f\x5f\x7f"_bin;
};

}


BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(in_port_test)

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const value = std::uint32_t{1};

        auto const sut = match::in_port{value};

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_IN_PORT);
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
        auto const value = std::uint32_t{0x01020304};
        auto const mask = std::uint32_t{0x0f0f0f0f};

        auto const sut = match::in_port{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_IN_PORT_W);
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
        auto const value = std::uint32_t{0};
        auto const mask = std::uint32_t{0};

        auto const sut = match::in_port{value, mask};

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
        auto const value = std::uint32_t{proto::OFPP_MAX};
        auto const mask = std::uint32_t{0xffffffff};

        auto const sut = match::in_port{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(no_mask_equality_test)
    {
        auto const sut = match::in_port{proto::OFPP_LOCAL};
        auto const same_value = match::in_port{proto::OFPP_LOCAL};
        auto const diff_value = match::in_port{proto::OFPP_CONTROLLER};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_AUTO_TEST_CASE(has_mask_equality_test)
    {
        auto const sut = match::in_port{1, 0x0000ffff};
        auto const same_value_and_same_mask = match::in_port{1, 0x0000ffff};
        auto const same_value_and_diff_mask = match::in_port{1, 0xffff0000};
        auto const diff_value_and_same_mask = match::in_port{2, 0x0000ffff};
        auto const diff_value_and_diff_mask = match::in_port{2, 0xffff0000};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value_and_same_mask));
        BOOST_TEST((sut != same_value_and_diff_mask));
        BOOST_TEST((sut != diff_value_and_same_mask));
        BOOST_TEST((sut != diff_value_and_diff_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_and_has_mask_equality_test)
    {
        auto const sut = match::in_port{3};
        auto const same_value_and_mask = match::in_port{3, 0x0f0f0f0f};
        auto const diff_value_and_mask = match::in_port{4, 0x0f0f0f0f};

        BOOST_TEST((sut != same_value_and_mask));
        BOOST_TEST((sut != diff_value_and_mask));
    }

    BOOST_AUTO_TEST_CASE(exact_and_wildcard_equality_test)
    {
        auto const sut = match::in_port{3};
        auto const same_value_and_all_zero_mask = match::in_port{3, 0x00000000};
        auto const same_value_and_all_one_mask = match::in_port{3, 0xffffffff};
        auto const diff_value_and_all_zero_mask = match::in_port{4, 0x00000000};
        auto const diff_value_and_all_one_mask = match::in_port{4, 0xffffffff};

        BOOST_TEST((sut != same_value_and_all_zero_mask));
        BOOST_TEST((sut == same_value_and_all_one_mask));
        BOOST_TEST((sut != diff_value_and_all_zero_mask));
        BOOST_TEST((sut != diff_value_and_all_one_mask));
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<std::uint32_t>{
                1, proto::OFPP_MAX, proto::OFPP_CONTROLLER, proto::OFPP_LOCAL
              })
            , value)
    {
        auto const sut = match::in_port::create(value);

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
            , bdata::make(std::vector<std::uint32_t>{
                1, proto::OFPP_MAX, proto::OFPP_CONTROLLER, proto::OFPP_LOCAL
              })
            ^ bdata::make(std::vector<std::uint32_t>{
                1, proto::OFPP_MAX, proto::OFPP_CONTROLLER, proto::OFPP_LOCAL
              })
            , value, mask)
    {
        auto const sut = match::in_port::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_failure_test
            , bdata::make(std::vector<std::uint32_t>{
                0, proto::OFPP_MAX + 1, proto::OFPP_ALL, proto::OFPP_ANY
              })
            , value)
    {
        BOOST_CHECK_THROW(match::in_port::create(value), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                match::in_port::create(0x0101, 0x1010), std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_encode_test, no_mask_in_port_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_decode_test, no_mask_in_port_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::in_port::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_encode_test, has_mask_in_port_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_decode_test, has_mask_in_port_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::in_port::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

BOOST_AUTO_TEST_SUITE_END() // in_port_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test

