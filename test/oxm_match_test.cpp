#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE oxm_match_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(oxm_match_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = oxm_match{};

        BOOST_CHECK_EQUAL(sut.type(), OFPMT_OXM);
        BOOST_CHECK_EQUAL(sut.length(), 4);
    }

    BOOST_AUTO_TEST_CASE(variadic_template_constructor_test)
    {
        auto lvalue = oxm_in_port{1};
        auto const clvalue = oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}};
        auto rvalue = oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}};
        auto const sut = oxm_match{
              oxm_in_port{2}, oxm_eth_type{0x0806}
            , lvalue, clvalue, std::move(rvalue), oxm_eth_type{0x0800}
        };

        BOOST_CHECK_EQUAL(sut.type(), OFPMT_OXM);
        BOOST_CHECK_EQUAL(sut.length(), 4 + 34);
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_test)
    {
        auto sut = oxm_match{
              oxm_in_port{1}
            , oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
            , oxm_eth_type{0x0800, 0x0800}
        };

        auto const copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = oxm_match{
              oxm_in_port{1}
            , oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
            , oxm_eth_type{0x0800, 0x0800}
        };
        auto const expected_length = sut.length();

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), expected_length);
        BOOST_CHECK_EQUAL(sut.length(), 4);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(assignment_test)

    BOOST_AUTO_TEST_CASE(copy_assign_test)
    {
        auto sut = oxm_match{
              oxm_in_port{1}
            , oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
            , oxm_eth_type{0x0800, 0x0800}
        }; // 8 + 10 + 10 + 8 = 36
        auto copy = oxm_match{
              oxm_in_phy_port{1}, oxm_ipv4_src{0x7f000001}, oxm_ipv4_dst{0x7f000002}
        }; // 8 + 8 + 8 = 24

        copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_assign_test)
    {
        auto sut = oxm_match{
              oxm_in_port{1}
            , oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
            , oxm_eth_type{0x0800, 0x0800}
        }; // 8 + 10 + 10 + 8 = 36
        auto copy = oxm_match{
              oxm_in_phy_port{1}, oxm_ipv4_src{0x7f000001}, oxm_ipv4_dst{0x7f000002}
        }; // 8 + 8 + 8 = 24

        copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), 4 + 36);
        BOOST_CHECK_EQUAL(sut.length(), 4);
    }

BOOST_AUTO_TEST_SUITE_END() // assignment_test

struct empty_oxm_match_fixture
{
    oxm_match sut{};
};
BOOST_FIXTURE_TEST_SUITE(when_init_state_is_empty, empty_oxm_match_fixture)

    BOOST_AUTO_TEST_CASE(add_oxm_match_field)
    {
        sut.add(oxm_vlan_vid{1024, 0x111});

        BOOST_CHECK_EQUAL(sut.length(), 12);

        auto const vlan_vid_field = sut.get<oxm_vlan_vid>();
        BOOST_REQUIRE(vlan_vid_field);
        BOOST_CHECK_EQUAL(vlan_vid_field->oxm_value(), 1024);
        BOOST_REQUIRE(vlan_vid_field->oxm_mask());
        // TODO mask has OFPVID_PRESENT?
        BOOST_CHECK_EQUAL(*vlan_vid_field->oxm_mask(), 0x111 | OFPVID_PRESENT);
    }

    BOOST_AUTO_TEST_CASE(get_null_oxm_match_field)
    {
        auto const vlan_vid_field = sut.get<oxm_vlan_vid>();

        BOOST_CHECK(!vlan_vid_field);
    }

    BOOST_AUTO_TEST_CASE(subscripting_get_null_oxm_match_field)
    {
        auto const in_port_field = sut[oxm_in_port::oxm_type()];

        BOOST_CHECK(!in_port_field);
    }

    BOOST_AUTO_TEST_CASE(encode_decode_test)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), 8);

        auto it = buffer.begin();
        auto const decoded_match = oxm_match::decode(it, buffer.end());

        BOOST_CHECK_EQUAL(decoded_match.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_match.length(), sut.length());
    }

BOOST_AUTO_TEST_SUITE_END() // when_init_state_is_empty

struct in_port_eth_type_ip_proto_matching_oxm_match_fixture
{
    oxm_match sut{
        oxm_in_port{4}, oxm_eth_type{0x0800}, oxm_ip_proto{3, 0x03}
    };
};
BOOST_FIXTURE_TEST_SUITE(when_init_state_has_some_oxm_match_field, in_port_eth_type_ip_proto_matching_oxm_match_fixture)

    BOOST_AUTO_TEST_CASE(add_new_oxm_match_field)
    {
        auto ipv4_dst = oxm_ipv4_dst{0x7f000001};

        sut.add(ipv4_dst);

        BOOST_CHECK_EQUAL(sut.length(), 4 + 20 + 8);
        auto const ipv4_dst_field = sut.get<oxm_ipv4_dst>();
        BOOST_REQUIRE(ipv4_dst_field);
        BOOST_CHECK_EQUAL(ipv4_dst_field->oxm_value(), 0x7f000001);
        BOOST_CHECK(!ipv4_dst_field->oxm_has_mask());
    }

    BOOST_AUTO_TEST_CASE(add_existing_field_then_the_field_is_updated)
    {
        auto const in_port = oxm_in_port{OFPP_MAX, 0x1111};
        auto const expected_length = sut.length();

        sut.add(in_port);

        BOOST_CHECK_EQUAL(sut.length(), expected_length + sizeof(std::uint32_t));
        auto const in_port_field = sut.get<oxm_in_port>();
        BOOST_REQUIRE(in_port_field);
        BOOST_CHECK_EQUAL(in_port_field->oxm_value(), OFPP_MAX);
        BOOST_REQUIRE(in_port_field->oxm_mask());
        BOOST_CHECK_EQUAL(*in_port_field->oxm_mask(), 0x1111);
    }

    BOOST_AUTO_TEST_CASE(encode_decode_test)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), detail::exact_length(sut.length()));

        auto it = buffer.begin();
        auto const decoded_match = oxm_match::decode(it, buffer.end());

        BOOST_CHECK_EQUAL(decoded_match.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_match.length(), sut.length());
    }

BOOST_AUTO_TEST_SUITE_END() // when_init_state_has_some_oxm_match_field

BOOST_AUTO_TEST_SUITE_END() // oxm_match_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
