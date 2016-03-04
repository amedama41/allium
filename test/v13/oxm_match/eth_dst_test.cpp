#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <canard/mac_address.hpp>

#include "../../test_utility.hpp"
#include "./oxm_match_test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;
using canard::mac_address;

namespace {

auto operator""_mac(char const* const str, std::size_t const size)
    -> canard::mac_address
{
    if (size != 6) {
        throw std::runtime_error{"invalid size mac address"};
    }
    auto const bytes = reinterpret_cast<unsigned char const*>(str);
    return canard::mac_address{
        {{bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]}}
    };
}

struct no_mask_eth_dst_fixture
{
    match::eth_dst sut{"\x01\x02\x03\x04\x05\x06"_mac};
    std::vector<std::uint8_t> binary
        = "\x80\x00\x06\x06\x01\x02\x03\x04""\x05\x06"_bin;
};

struct has_mask_eth_dst_fixture
{
    match::eth_dst sut{
        "\x01\x02\x03\x04\x05\x06"_mac, "\x0f\x1f\x2f\x3f\x4f\x5f"_mac
    };
    std::vector<std::uint8_t> binary
        = "\x80\x00\x07\x0c\x01\x02\x03\x04""\x05\x06\x0f\x1f\x2f\x3f\x4f\x5f"
          ""_bin;
};

}

BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(eth_dst_test)

    BOOST_AUTO_TEST_CASE(construct_from_value_test)
    {
        auto const value = "\x01\x02\x03\x04\x05\x06"_mac;

        auto const sut = match::eth_dst{value};

        BOOST_TEST(sut.oxm_length() == sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_header() == OXM_OF_ETH_DST);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(mac_address::bytes_type));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_value_and_mask_test)
    {
        auto const value = "\x11\x12\x13\x14\x15\x16"_mac;
        auto const mask = "\x0f\x0f\x0f\x0f\x0f\x0f"_mac;

        auto const sut = match::eth_dst{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_header() == OXM_OF_ETH_DST_W);
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_zero_mask_test)
    {
        auto const value = "\x00\x00\x00\x00\x00\x00"_mac;
        auto const mask = "\x00\x00\x00\x00\x00\x00"_mac;

        auto const sut = match::eth_dst{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_one_mask_test)
    {
        auto const value = "\xa1\xa2\xa3\xa4\xa5\xa6"_mac;
        auto const mask = "\xff\xff\xff\xff\xff\xff"_mac;

        auto const sut = match::eth_dst{value, mask};

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(no_mask_equality_test)
    {
        auto const sut = match::eth_dst{"\x21\x22\x23\x24\x25\x26"_mac};
        auto const same_value = match::eth_dst{"\x21\x22\x23\x24\x25\x26"_mac};
        auto const diff_value = match::eth_dst{"\x31\x32\x33\x34\x35\x36"_mac};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_AUTO_TEST_CASE(has_mask_equality_test)
    {
        auto const sut = match::eth_dst{
            "\x01\x02\x03\x04\x05\x06"_mac, "\xff\x0f\xff\x0f\xff\x0f"_mac
        };
        auto const same_value_and_same_mask = match::eth_dst{
            "\x01\x02\x03\x04\x05\x06"_mac, "\xff\x0f\xff\x0f\xff\x0f"_mac
        };
        auto const same_value_and_diff_mask = match::eth_dst{
            "\x01\x02\x03\x04\x05\x06"_mac, "\x0f\xff\x0f\xff\x0f\xff"_mac
        };
        auto const diff_value_and_same_mask = match::eth_dst{
            "\x11\x12\x13\x14\x15\x16"_mac, "\xff\x0f\xff\x0f\xff\x0f"_mac
        };
        auto const diff_value_and_diff_mask = match::eth_dst{
            "\x11\x12\x13\x14\x15\x16"_mac, "\x0f\xff\x0f\xff\x0f\xff"_mac
        };

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value_and_same_mask));
        BOOST_TEST((sut != same_value_and_diff_mask));
        BOOST_TEST((sut != diff_value_and_same_mask));
        BOOST_TEST((sut != diff_value_and_diff_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_and_has_mask_equality_test)
    {
        auto const sut = match::eth_dst{"\x41\x42\x43\x44\x45\x46"_mac};
        auto const same_value_and_mask = match::eth_dst{
            "\x41\x42\x43\x44\x45\x46"_mac, "\xff\x00\xff\x00\xff\x00"_mac
        };
        auto const diff_value_and_mask = match::eth_dst{
            "\x51\x52\x53\x54\x55\x46"_mac, "\xff\x00\xff\x00\xff\x00"_mac
        };

        BOOST_TEST((sut != same_value_and_mask));
        BOOST_TEST((sut != diff_value_and_mask));
    }

    BOOST_AUTO_TEST_CASE(exact_and_wildcard_equality_test)
    {
        auto const sut = match::eth_dst{"\x41\x42\x43\x44\x45\x46"_mac};
        auto const same_value_and_all_zero_mask = match::eth_dst{
            "\x41\x42\x43\x44\x45\x46"_mac, "\x00\x00\x00\x00\x00\x00"_mac
        };
        auto const same_value_and_all_one_mask = match::eth_dst{
            "\x41\x42\x43\x44\x45\x46"_mac, "\xff\xff\xff\xff\xff\xff"_mac
        };
        auto const diff_value_and_all_zero_mask = match::eth_dst{
            "\x51\x52\x53\x54\x55\x56"_mac, "\x00\x00\x00\x00\x00\x00"_mac
        };
        auto const diff_value_and_all_one_mask = match::eth_dst{
            "\x51\x52\x53\x54\x55\x56"_mac, "\xff\xff\xff\xff\xff\xff"_mac
        };

        BOOST_TEST((sut != same_value_and_all_zero_mask));
        BOOST_TEST((sut == same_value_and_all_one_mask));
        BOOST_TEST((sut != diff_value_and_all_zero_mask));
        BOOST_TEST((sut != diff_value_and_all_one_mask));
    }

    BOOST_AUTO_TEST_CASE(no_mask_create_success_test)
    {
        auto const value = "\x01\x02\x03\x04\x05\x06"_mac;

        auto const sut = match::eth_dst::create(value);

        BOOST_TEST(sut.oxm_length() == sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(mac_address::bytes_type));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_success_test)
    {
        auto const value = "\x01\x02\x03\x04\x05\x06"_mac;
        auto const mask = "\x0f\x0f\x0f\x0f\x0f\x0f"_mac;

        auto const sut = match::eth_dst::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(mac_address::bytes_type));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_AUTO_TEST_CASE(create_failure_test)
    {
        auto const value = "\x01\x02\x03\x04\x01\x02"_mac;
        auto const mask = "\x01\x02\x02\x04\x05\x06"_mac;

        BOOST_CHECK_THROW(
                match::eth_dst::create(value, mask), std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_encode_test, no_mask_eth_dst_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(no_mask_decode_test, no_mask_eth_dst_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::eth_dst::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_encode_test, has_mask_eth_dst_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(has_mask_decode_test, has_mask_eth_dst_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = match::eth_dst::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
        BOOST_TEST(field.oxm_has_mask() == sut.oxm_has_mask());
        BOOST_TEST(field.oxm_length() == sut.oxm_length());
        BOOST_TEST(field.oxm_value() == sut.oxm_value());
        BOOST_TEST(field.oxm_mask() == sut.oxm_mask());
    }

BOOST_AUTO_TEST_SUITE_END() // eth_dst_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
