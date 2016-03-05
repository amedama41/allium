#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <cstdint>
#include <utility>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;

using boost::asio::ip::address_v4;
using boost::asio::ip::address_v6;
using canard::mac_address;

namespace bdata = boost::unit_test::data;

BOOST_TEST_DONT_PRINT_LOG_VALUE(v13::any_oxm_match_field)
BOOST_TEST_DONT_PRINT_LOG_VALUE(std::vector<std::uint8_t>)

namespace {

    std::vector<v13::any_oxm_match_field> fields = {
          v13::any_oxm_match_field{match::in_port{0x01020304}}
        , v13::any_oxm_match_field{match::in_port{0x01020304, 0x0f0f0f0f}}
        , v13::any_oxm_match_field{match::eth_dst{
            "\x01\x02\x03\x04\x05\x06"_mac
          }}
        , v13::any_oxm_match_field{match::eth_dst{
            "\x01\x02\x03\x04\x05\x06"_mac, "\x0f\x0f\x0f\x0f\x0f\x0f"_mac
          }}
        , v13::any_oxm_match_field{match::ipv4_dst{
            address_v4::from_string("192.168.10.128")
          }}
        , v13::any_oxm_match_field{match::ipv4_dst{
            address_v4::from_string("192.168.10.128"), 25
          }}
        , v13::any_oxm_match_field{match::ipv6_dst{
            address_v6::from_string("2001:db8::9abc:8000")
          }}
        , v13::any_oxm_match_field{match::ipv6_dst{
            address_v6::from_string("2001:db8::9abc:8000"), 113
          }}
    };

    std::vector<std::vector<std::uint8_t>> binary_fields = {
          "\x80\x00\x00\x04\x01\x02\x03\x04"_bin
        , "\x80\x00\x01\x08\x01\x02\x03\x04""\x0f\x0f\x0f\x0f"_bin
        , "\x80\x00\x06\x06\x01\x02\x03\x04""\x05\x06"_bin
        , "\x80\x00\x07\x0c\x01\x02\x03\x04""\x05\x06\x0f\x0f\x0f\x0f\x0f\x0f"_bin
        , "\x80\x00\x18\x04\xc0\xa8\x0a\x80"_bin
        , "\x80\x00\x19\x08\xc0\xa8\x0a\x80""\xff\xff\xff\x80"_bin
        , "\x80\x00\x36\x10\x20\x01\x0d\xb8""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x9a\xbc\x80\x00"_bin
        , "\x80\x00\x37\x20\x20\x01\x0d\xb8""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x9a\xbc\x80\x00\xff\xff\xff\xff""\xff\xff\xff\xff\xff\xff\xff\xff"
          "\xff\xff\x80\x00"_bin
    };

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(any_oxm_match_field_test)

    BOOST_AUTO_TEST_CASE(construct_from_no_mask_field_test)
    {
        auto const field = match::in_port{1};

        auto const sut = v13::any_oxm_match_field{field};

        BOOST_TEST(sut.oxm_class() == field.oxm_class());
        BOOST_TEST(sut.oxm_field() == field.oxm_field());
        BOOST_TEST(sut.oxm_type() == field.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field.oxm_length());
        BOOST_TEST(sut.length() == field.length());
        BOOST_TEST(sut.is_wildcard() == field.is_wildcard());
        BOOST_TEST(sut.is_exact() == field.is_exact());
        BOOST_TEST((v13::any_cast<match::in_port>(sut) == field));
    }

    BOOST_AUTO_TEST_CASE(construct_from_has_mask_field_test)
    {
        auto const field
            = match::vlan_vid{proto::OFPVID_PRESENT, proto::OFPVID_PRESENT};

        auto const sut = v13::any_oxm_match_field{field};

        BOOST_TEST(sut.oxm_class() == field.oxm_class());
        BOOST_TEST(sut.oxm_field() == field.oxm_field());
        BOOST_TEST(sut.oxm_type() == field.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field.oxm_length());
        BOOST_TEST(sut.length() == field.length());
        BOOST_TEST(sut.is_wildcard() == field.is_wildcard());
        BOOST_TEST(sut.is_exact() == field.is_exact());
        BOOST_TEST((v13::any_cast<match::vlan_vid>(sut) == field));
    }

    BOOST_AUTO_TEST_CASE(assign_from_no_mask_same_type_field_test)
    {
        auto const field1
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const field2
            = match::ipv4_src{address_v4::from_string("172.16.0.0")};
        auto sut = v13::any_oxm_match_field{field1};

        sut = field2;

        BOOST_TEST(sut.oxm_class() == field2.oxm_class());
        BOOST_TEST(sut.oxm_field() == field2.oxm_field());
        BOOST_TEST(sut.oxm_type() == field2.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field2.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field2.oxm_length());
        BOOST_TEST(sut.length() == field2.length());
        BOOST_TEST(sut.is_wildcard() == field2.is_wildcard());
        BOOST_TEST(sut.is_exact() == field2.is_exact());
        BOOST_TEST((v13::any_cast<match::ipv4_src>(sut) == field2));
    }

    BOOST_AUTO_TEST_CASE(assign_from_has_mask_same_type_field_test)
    {
        auto const field1
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const field2
            = match::ipv4_src{address_v4::from_string("172.16.0.0"), 16};
        auto sut = v13::any_oxm_match_field{field1};

        sut = field2;

        BOOST_TEST(sut.oxm_class() == field2.oxm_class());
        BOOST_TEST(sut.oxm_field() == field2.oxm_field());
        BOOST_TEST(sut.oxm_type() == field2.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field2.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field2.oxm_length());
        BOOST_TEST(sut.length() == field2.length());
        BOOST_TEST(sut.is_wildcard() == field2.is_wildcard());
        BOOST_TEST(sut.is_exact() == field2.is_exact());
        BOOST_TEST((v13::any_cast<match::ipv4_src>(sut) == field2));
    }

    BOOST_AUTO_TEST_CASE(assign_from_no_mask_diff_type_field_test)
    {
        auto const field1
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const field2
            = match::ipv6_src{address_v6::from_string("::1")};
        auto sut = v13::any_oxm_match_field{field1};

        sut = field2;

        BOOST_TEST(sut.oxm_class() == field2.oxm_class());
        BOOST_TEST(sut.oxm_field() == field2.oxm_field());
        BOOST_TEST(sut.oxm_type() == field2.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field2.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field2.oxm_length());
        BOOST_TEST(sut.length() == field2.length());
        BOOST_TEST(sut.is_wildcard() == field2.is_wildcard());
        BOOST_TEST(sut.is_exact() == field2.is_exact());
        BOOST_TEST((v13::any_cast<match::ipv6_src>(sut) == field2));
    }

    BOOST_AUTO_TEST_CASE(assign_from_has_mask_diff_type_field_test)
    {
        auto const field1
            = match::ipv4_src{address_v4::from_string("192.168.1.0"), 24};
        auto const field2
            = match::ipv6_src{address_v6::from_string("::1"), 64};
        auto sut = v13::any_oxm_match_field{field1};

        sut = field2;

        BOOST_TEST(sut.oxm_class() == field2.oxm_class());
        BOOST_TEST(sut.oxm_field() == field2.oxm_field());
        BOOST_TEST(sut.oxm_type() == field2.oxm_type());
        BOOST_TEST(sut.oxm_has_mask() == field2.oxm_has_mask());
        BOOST_TEST(sut.oxm_length() == field2.oxm_length());
        BOOST_TEST(sut.length() == field2.length());
        BOOST_TEST(sut.is_wildcard() == field2.is_wildcard());
        BOOST_TEST(sut.is_exact() == field2.is_exact());
        BOOST_TEST((v13::any_cast<match::ipv6_src>(sut) == field2));
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = v13::any_oxm_match_field{match::tcp_src{1}};
        auto const same_value = v13::any_oxm_match_field{match::tcp_src{1}};
        auto const diff_value = v13::any_oxm_match_field{match::tcp_src{2}};
        auto const has_mask = v13::any_oxm_match_field{match::tcp_src{1, 1}};
        auto const diff_type = v13::any_oxm_match_field{match::udp_dst{1}};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != has_mask));
        BOOST_TEST((sut != diff_type));
    }

    BOOST_DATA_TEST_CASE(
              encode_test
            , bdata::make(fields) ^ bdata::make(binary_fields)
            , sut, binary)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_DATA_TEST_CASE(
              decode_test
            , bdata::make(fields) ^ bdata::make(binary_fields)
            , sut, binary)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const field = v13::any_oxm_match_field::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((field == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // any_oxm_match_field_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
