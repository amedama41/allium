#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <stdexcept>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "../../test_utility.hpp"
#include "./oxm_match_test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;

namespace bdata = boost::unit_test::data;

namespace {

struct no_mask_vlan_vid_fixture
{
    match::vlan_vid sut{0x0123 | proto::OFPVID_PRESENT};
    std::vector<std::uint8_t> binary = "\x80\x00\x0c\x02\x11\x23"_bin;
};

struct has_mask_vlan_vid_fixture
{
    match::vlan_vid sut{
          0x0123 | proto::OFPVID_PRESENT
        , 0x0f2f | proto::OFPVID_PRESENT
    };
    std::vector<std::uint8_t> binary = "\x80\x00\x0d\x04\x11\x23\x1f\x2f"_bin;
};

}

BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(vlan_vid_test)

    BOOST_DATA_TEST_CASE(
              construct_from_value_test
            , bdata::make(std::vector<std::uint16_t>{
                0x0001 | proto::OFPVID_PRESENT, proto::OFPVID_NONE
              })
            , value)
    {
        auto const sut = match::vlan_vid{value};

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_VLAN_VID);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              construct_from_value_and_mask_test
            , bdata::make(std::vector<std::uint16_t>{
                proto::OFPVID_PRESENT | 0x123, proto::OFPVID_PRESENT
              })
            ^ bdata::make(std::vector<std::uint16_t>{
                proto::OFPVID_PRESENT | 0xf2f, proto::OFPVID_PRESENT
              })
            , value, mask)
    {
        auto const sut = match::vlan_vid{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_header() == OXM_OF_VLAN_VID_W);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_zero_mask_test)
    {
        auto const value = std::uint16_t{proto::OFPVID_NONE};
        auto const mask = std::uint16_t{0};

        auto const sut = match::vlan_vid{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_one_mask_test)
    {
        auto const value = std::uint16_t{proto::OFPVID_PRESENT | 0xfff};
        auto const mask = std::uint16_t{0xffff};

        auto const sut = match::vlan_vid{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(no_mask_equality_test)
    {
        auto const sut = match::vlan_vid{proto::OFPVID_PRESENT | 2};
        auto const same_value = match::vlan_vid{proto::OFPVID_PRESENT | 2};
        auto const diff_value = match::vlan_vid{proto::OFPVID_PRESENT | 3};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_AUTO_TEST_CASE(has_mask_equality_test)
    {
        auto const sut = match::vlan_vid{proto::OFPVID_PRESENT | 3, 0x00ff};
        auto const same_value_and_same_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 3, 0x00ff};
        auto const same_value_and_diff_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 3, 0xff00};
        auto const diff_value_and_same_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 4, 0x00ff};
        auto const diff_value_and_diff_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 4, 0xff00};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value_and_same_mask));
        BOOST_TEST((sut != same_value_and_diff_mask));
        BOOST_TEST((sut != diff_value_and_same_mask));
        BOOST_TEST((sut != diff_value_and_diff_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_and_has_mask_equality_test)
    {
        auto const sut = match::vlan_vid{proto::OFPVID_PRESENT | 0x0400};
        auto const same_value_and_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0400, 0x0f0f};
        auto const diff_value_and_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0401, 0x0f0f};

        BOOST_TEST((sut != same_value_and_mask));
        BOOST_TEST((sut != diff_value_and_mask));
    }

    BOOST_AUTO_TEST_CASE(exact_and_wildcard_equality_test)
    {
        auto const sut = match::vlan_vid{proto::OFPVID_PRESENT | 0x0400};
        auto const same_value_and_all_zero_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0400, 0x0000};
        auto const same_value_and_all_one_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0400, 0xffff};
        auto const diff_value_and_all_zero_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0401, 0x0000};
        auto const diff_value_and_all_one_mask
            = match::vlan_vid{proto::OFPVID_PRESENT | 0x0401, 0xffff};

        BOOST_TEST((sut != same_value_and_all_zero_mask));
        BOOST_TEST((sut == same_value_and_all_one_mask));
        BOOST_TEST((sut != diff_value_and_all_zero_mask));
        BOOST_TEST((sut != diff_value_and_all_one_mask));
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<std::uint16_t>{
                  0x0001 | proto::OFPVID_PRESENT
                , 0x0fff | proto::OFPVID_PRESENT
                , proto::OFPVID_NONE
              })
            , value)
    {
        auto const sut = match::vlan_vid::create(value);

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              has_mask_create_success_test
            , bdata::make(std::vector<std::uint16_t>{
                  0x0001 | proto::OFPVID_PRESENT
                , 0x0fff | proto::OFPVID_PRESENT
                , proto::OFPVID_NONE
              })
            ^ bdata::make(std::vector<std::uint16_t>{
                  0x0001 | proto::OFPVID_PRESENT
                , 0x0fff | proto::OFPVID_PRESENT
                , 0x0001
              })
            , value, mask)
    {
        auto const sut = match::vlan_vid::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_failure_test
            , bdata::make(std::vector<std::uint16_t>{
                0x0fff, proto::OFPVID_PRESENT | 0x2000, 0xffff
              })
            , value)
    {
        BOOST_CHECK_THROW(match::vlan_vid::create(value), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                match::vlan_vid::create(0x1101, 0x1010), std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_encode_test, no_mask_vlan_vid_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_decode_test, no_mask_vlan_vid_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::vlan_vid::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_encode_test, has_mask_vlan_vid_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_decode_test, has_mask_vlan_vid_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::vlan_vid::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

BOOST_AUTO_TEST_SUITE_END() // vlan_vid_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
