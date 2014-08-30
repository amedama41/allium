#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE set_field_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <utility>
#include <boost/type_erasure/any_cast.hpp>
#include <canard/network/protocol/openflow/v13/action/set_field.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(set_field_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const oxm = oxm_eth_type{0x0806};

        auto const sut = actions::set_field{oxm};

        BOOST_CHECK_EQUAL(sut.type(), OFPAT_SET_FIELD);
        BOOST_CHECK_EQUAL(sut.length(), ((4 + oxm.length()) + 7) / 8 * 8);
        BOOST_CHECK_EQUAL(sut.length() % 8, 0);
        BOOST_CHECK_EQUAL(sut.oxm_match_field().oxm_type(), oxm.oxm_type());
        auto const sut_field = boost::type_erasure::any_cast<oxm_eth_type const*>(&sut.oxm_match_field());
        BOOST_REQUIRE(sut_field);
        BOOST_CHECK_EQUAL(sut_field->oxm_value(), oxm.oxm_value());
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_test)
    {
        auto sut = actions::set_field{oxm_ipv6_src{{{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0xd, 0x0e, 0x0f}}}};

        auto const copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(construct_with_any_oxm_match_field)
    {
        auto oxm = any_oxm_match_field{oxm_vlan_vid{0x0fff}};

        auto const sut = actions::set_field{std::move(oxm)};

        BOOST_CHECK_EQUAL(sut.type(), OFPAT_SET_FIELD);
        BOOST_CHECK_EQUAL(sut.length(), ((4 + sizeof(std::uint32_t) + sizeof(std::uint16_t)) + 7) / 8 * 8);
        BOOST_CHECK_EQUAL(sut.length() % 8, 0);
        BOOST_CHECK_EQUAL(sut.oxm_match_field().oxm_type(), oxm_vlan_vid::oxm_type());
        auto const sut_field = boost::type_erasure::any_cast<oxm_vlan_vid const*>(&sut.oxm_match_field());
        BOOST_REQUIRE(sut_field);
        BOOST_CHECK_EQUAL(sut_field->oxm_value(), 0x0fff);
    }

    BOOST_AUTO_TEST_CASE(not_construct_with_empty_oxm_match_field)
    {
#ifdef CANARD_CHECK_THIS_CODE_SHALL_BE_COMPILE_ERROR
        auto const sut = actions::set_field{any_oxm_match_field{}};
        BOOST_ERROR("set_field must not be constructed with empty any_oxm_match_field");
#endif
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(assignment_test)

    BOOST_AUTO_TEST_CASE(copy_assign_test)
    {
        auto sut = actions::set_field{oxm_ipv4_src{0x7f000001}};
        auto copy = actions::set_field{oxm_ipv4_src{0x7f000002}};

        copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.oxm_match_field().oxm_type(), oxm_ipv4_src::oxm_type());
        auto const field = boost::type_erasure::any_cast<oxm_ipv4_src const*>(&copy.oxm_match_field());
        BOOST_REQUIRE(field);
        BOOST_CHECK_EQUAL(field->oxm_value(), 0x7f000001);
    }

    BOOST_AUTO_TEST_CASE(move_assign_test)
    {
        auto sut = actions::set_field{oxm_ipv4_src{0x7f000001}};
        auto copy = actions::set_field{oxm_ipv4_src{0x7f000002}};

        copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.oxm_match_field().oxm_type(), oxm_ipv4_src::oxm_type());
        auto const field = boost::type_erasure::any_cast<oxm_ipv4_src const*>(&copy.oxm_match_field());
        BOOST_REQUIRE(field);
        BOOST_CHECK_EQUAL(field->oxm_value(), 0x7f000001);
    }

BOOST_AUTO_TEST_SUITE_END() // assignment_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::set_field{oxm_ipv4_src{0x7f000001}};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::set_field::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.oxm_match_field().oxm_type(), oxm_ipv4_src::oxm_type());
    auto const decode_action_field = boost::type_erasure::any_cast<oxm_ipv4_src const*>(&sut.oxm_match_field());
    BOOST_REQUIRE(decode_action_field);
    BOOST_CHECK_EQUAL(decode_action_field->oxm_value(), 0x7f000001);
}

BOOST_AUTO_TEST_SUITE_END() // set_field_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
