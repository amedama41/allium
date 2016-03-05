#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;
namespace detail = v13::v13_detail;

using proto = v13::protocol;

namespace {

struct oxm_match_set_fixture
{
    v13::oxm_match_set sut{
          match::in_port{proto::OFPP_MAX} // 8
        , match::eth_dst{"\x01\x02\x03\x04\x05\x06"_mac} // 10
        , match::eth_type{0x0800} // 6
        , match::vlan_vid{proto::OFPVID_PRESENT, proto::OFPVID_PRESENT} // 8
        , match::ip_proto{6} // 5
        , match::ipv4_src{"192.168.10.0"_ipv4, 24} // 12
    }; // 4 + (8 + 10 + 6 + 8 + 5 + 12) = 53
    std::vector<std::uint8_t> binary
        = "\x00\x01\x00\x35\x80\x00\x00\x04""\xff\xff\xff\x00\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x0a\x02\x08\x00\x80\x00\x0d\x04"
          "\x10\x00\x10\x00\x80\x00\x14\x01""\x06\x80\x00\x17\x08\xc0\xa8\x0a"
          "\x00\xff\xff\xff\x00\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(oxm_match_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::oxm_match_set{};

        BOOST_TEST(sut.type() == proto::OFPMT_OXM);
        BOOST_TEST(sut.length() == 4);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_fields_test)
    {
        auto const in_phy_port = match::in_phy_port{1, 1}; // 12

        auto const sut = v13::oxm_match_set{in_phy_port};

        BOOST_TEST(sut.length() == 4 + 12);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 1);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST_REQUIRE(bool(sut.find<match::in_phy_port>()));
        BOOST_TEST((sut.find(in_phy_port.oxm_type()) != sut.end()));
        BOOST_TEST((sut.at(in_phy_port.oxm_type()) == in_phy_port));
        BOOST_TEST((sut.get<match::in_phy_port>() == in_phy_port));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_fields_test)
    {
        auto const in_port = match::in_port{1}; // 8
        auto const eth_dst = match::eth_dst{ // 10
            "\x01\x02\x03\x04\x05\x06"_mac
        };
        auto const eth_src = match::eth_src{ // 16
            "\x11\x12\x13\x14\x15\x16"_mac, "\xff\x12\xff\x14\xff\x16"_mac
        };
        auto const ipv4_src = match::ipv4_src{ // 12
            "192.168.1.0"_ipv4, 24
        };

        auto const sut = v13::oxm_match_set{
            in_port, eth_dst, eth_src, ipv4_src
        };

        BOOST_TEST(sut.length() == 4 + 8 + 10 + 16 + 12);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 4);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST_REQUIRE(bool(sut.find<match::in_port>()));
        BOOST_TEST((sut.get<match::in_port>() == in_port));

        BOOST_TEST_REQUIRE(bool(sut.find<match::eth_dst>()));
        BOOST_TEST((sut.get<match::eth_dst>() == eth_dst));

        BOOST_TEST_REQUIRE(bool(sut.find<match::eth_src>()));
        BOOST_TEST((sut.get<match::eth_src>() == eth_src));

        BOOST_TEST_REQUIRE(bool(sut.find<match::ipv4_src>()));
        BOOST_TEST((sut.get<match::ipv4_src>() == ipv4_src));
    }

    BOOST_AUTO_TEST_CASE(construct_from_same_fields_test)
    {
        auto const in_port1 = match::in_port{1}; // 8
        auto const eth_type = match::eth_type{0x0800}; // 6
        auto const in_port2 = match::in_port{2, 2}; // 12

        auto const sut = v13::oxm_match_set{in_port1, eth_type, in_port2};

        BOOST_TEST(sut.length() == 4 + 8 + 6);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 2);

        BOOST_TEST_REQUIRE(bool(sut.find<match::in_port>()));
        BOOST_TEST((sut.get<match::in_port>() == in_port1));

        BOOST_TEST_REQUIRE(bool(sut.find<match::eth_type>()));
        BOOST_TEST((sut.get<match::eth_type>() == eth_type));
    }

    BOOST_AUTO_TEST_SUITE(equality_test)

        BOOST_AUTO_TEST_CASE(no_mask_same_field_types_test)
        {
            auto const sut1
                = v13::oxm_match_set{match::in_port{1}, match::vlan_pcp{1}};
            auto const sut2
                = v13::oxm_match_set{match::in_port{2}, match::vlan_pcp{1}};
            auto const sut3
                = v13::oxm_match_set{match::in_port{1}, match::vlan_pcp{2}};
            auto const sut4
                = v13::oxm_match_set{match::in_port{2}, match::vlan_pcp{2}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
            BOOST_TEST((sut1 != sut4));
        }

        BOOST_AUTO_TEST_CASE(has_mask_same_field_types_test)
        {
            auto const sut1
                = v13::oxm_match_set{match::in_port{1, 3}, match::vlan_pcp{1}};
            auto const sut2
                = v13::oxm_match_set{match::in_port{2, 3}, match::vlan_pcp{1}};
            auto const sut3
                = v13::oxm_match_set{match::in_port{1, 3}, match::vlan_pcp{2}};
            auto const sut4
                = v13::oxm_match_set{match::in_port{2, 3}, match::vlan_pcp{2}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
            BOOST_TEST((sut1 != sut4));
        }

        BOOST_AUTO_TEST_CASE(differenct_field_types_test)
        {
            auto const sut1
                = v13::oxm_match_set{match::in_port{1}, match::vlan_pcp{1}};
            auto const sut2
                = v13::oxm_match_set{match::in_port{2}, match::vlan_vid{1}};
            auto const sut3
                = v13::oxm_match_set{match::in_phy_port{1}, match::vlan_pcp{2}};
            auto const sut4
                = v13::oxm_match_set{match::in_phy_port{2}, match::vlan_vid{2}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
            BOOST_TEST((sut1 != sut4));
        }

        BOOST_AUTO_TEST_CASE(has_wildcard_field_test)
        {
            auto const sut1
                = v13::oxm_match_set{match::in_port{1}, match::vlan_pcp{0, 0}};
            auto const sut2
                = v13::oxm_match_set{match::in_port{1}, match::vlan_vid{0, 0}};

            BOOST_TEST((sut1 == sut2));
        }

        BOOST_AUTO_TEST_CASE(empty_match_test)
        {
            auto const sut1 = v13::oxm_match_set{};
            auto const sut2 = v13::oxm_match_set{match::in_port{1}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
        }

    BOOST_AUTO_TEST_SUITE_END() // equality_test

    BOOST_FIXTURE_TEST_CASE(insert_new_field_test, oxm_match_set_fixture)
    {
        auto const tcp_src = match::tcp_src{6653};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(tcp_src);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == tcp_src));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + tcp_src.length());
        BOOST_TEST_REQUIRE(bool(sut.find<match::tcp_src>()));
        BOOST_TEST((sut.get<match::tcp_src>() == tcp_src));
    }

    BOOST_FIXTURE_TEST_CASE(insert_existing_field_test, oxm_match_set_fixture)
    {
        auto const ipv4_src = match::ipv4_src{"127.0.0.1"_ipv4};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(ipv4_src);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first != ipv4_src));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(bool(sut.find<match::ipv4_src>()));
        BOOST_TEST((sut.get<match::ipv4_src>() != ipv4_src));
    }

    BOOST_FIXTURE_TEST_CASE(assign_new_field_test, oxm_match_set_fixture)
    {
        auto const tcp_src = match::tcp_src{6653};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.assign(tcp_src);

        BOOST_TEST(!result.second);
        BOOST_TEST((result.first == sut.end()));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST(!sut.find<match::tcp_src>());
    }

    BOOST_FIXTURE_TEST_CASE(assign_existing_field_test, oxm_match_set_fixture)
    {
        auto const ipv4_src = match::ipv4_src{"127.0.0.1"_ipv4};
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const assigned_field_length
            = sut.at(match::ipv4_src::oxm_type()).length();

        auto const result = sut.assign(ipv4_src);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == ipv4_src));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length()
                == before_length - assigned_field_length + ipv4_src.length());
        BOOST_TEST_REQUIRE(bool(sut.find<match::ipv4_src>()));
        BOOST_TEST((sut.get<match::ipv4_src>() == ipv4_src));
    }

    BOOST_FIXTURE_TEST_CASE(
            insert_or_assign_new_field_test, oxm_match_set_fixture)
    {
        auto const tcp_src = match::tcp_src{6653};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert_or_assign(tcp_src);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == tcp_src));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + tcp_src.length());
        BOOST_TEST_REQUIRE(bool(sut.find<match::tcp_src>()));
        BOOST_TEST((sut.get<match::tcp_src>() == tcp_src));
    }

    BOOST_FIXTURE_TEST_CASE(
            insert_or_assign_existing_field_test, oxm_match_set_fixture)
    {
        auto const ipv4_src = match::ipv4_src{"127.0.0.1"_ipv4};
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const assigned_field_length
            = sut.at(match::ipv4_src::oxm_type()).length();

        auto const result = sut.insert_or_assign(ipv4_src);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first == ipv4_src));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length()
                == before_length - assigned_field_length + ipv4_src.length());
        BOOST_TEST_REQUIRE(bool(sut.find<match::ipv4_src>()));
        BOOST_TEST((sut.get<match::ipv4_src>() == ipv4_src));
    }

    BOOST_FIXTURE_TEST_CASE(erase_new_field_test, oxm_match_set_fixture)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.erase<match::ip_dscp>();

        BOOST_TEST(result == 0);
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST(!sut.find<match::ip_dscp>());
    }

    BOOST_FIXTURE_TEST_CASE(erase_existing_field_test, oxm_match_set_fixture)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_field_length
            = sut.at(match::vlan_vid::oxm_type()).length();

        auto const result = sut.erase<match::vlan_vid>();

        BOOST_TEST(result == 1);
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_field_length);
        BOOST_TEST(!sut.find<match::vlan_vid>());
    }

    BOOST_FIXTURE_TEST_CASE(erase_by_iterator_test, oxm_match_set_fixture)
    {
        auto it = std::prev(sut.end());
        auto const erased_oxm_type = it->oxm_type();
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_field_length = it->length();

        auto const result = sut.erase(it);

        BOOST_TEST((result == sut.end()));
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_field_length);
        BOOST_TEST((sut.find(erased_oxm_type) == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(clear_test, oxm_match_set_fixture)
    {
        sut.clear();

        BOOST_TEST(sut.length() == 4);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, oxm_match_set_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == detail::exact_length(sut.length()));
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, oxm_match_set_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const match_set = v13::oxm_match_set::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((match_set == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
