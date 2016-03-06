#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/push_vlan.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;
namespace v13_detail = v13::v13_detail;

using protocol = v13::protocol;

namespace bdata = boost::unit_test::data;

namespace {

struct push_vlan_fixture
{
    actions::push_vlan sut{0x8100};
    std::vector<std::uint8_t> binary = "\x00\x11\x00\x08\x81\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(action_test)
BOOST_AUTO_TEST_SUITE(push_vlan_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::push_vlan;

        BOOST_TEST(sut::type() == protocol::OFPAT_PUSH_VLAN);
        BOOST_TEST(sut::length() == sizeof(v13_detail::ofp_action_push));
    }

    BOOST_DATA_TEST_CASE(
              construct_test
            , bdata::make(std::vector<std::uint16_t>{0x8100, 0x88a8})
            , ethertype)
    {
        auto const sut = actions::push_vlan{ethertype};

        BOOST_TEST(sut.ethertype() == ethertype);
    }

    BOOST_AUTO_TEST_CASE(ieee802_1Q_test)
    {
        auto const sut = actions::push_vlan::ieee802_1Q();

        BOOST_TEST(sut.ethertype() == 0x8100);
    }

    BOOST_AUTO_TEST_CASE(ieee802_1ad_test)
    {
        auto const sut = actions::push_vlan::ieee802_1ad();

        BOOST_TEST(sut.ethertype() == 0x88a8);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint16_t>{0x8100, 0x88a8})
            , ethertype)
    {
        auto const sut = actions::push_vlan::create(ethertype);

        BOOST_TEST(sut.ethertype() == ethertype);
    }

    BOOST_DATA_TEST_CASE(
            create_failure_test
            , bdata::make(std::vector<std::uint16_t>{
                0x80ff, 0x8101, 0x88a7, 0x88a9
              })
            , ethertype)
    {
        BOOST_CHECK_THROW(
                actions::push_vlan::create(ethertype), std::runtime_error);
        BOOST_CHECK_THROW(
                actions::push_vlan::create(ethertype), std::runtime_error);
        BOOST_CHECK_THROW(
                actions::push_vlan::create(ethertype), std::runtime_error);
        BOOST_CHECK_THROW(
                actions::push_vlan::create(ethertype), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const ieee802_1Q = actions::push_vlan::ieee802_1Q();
        auto const ieee802_1ad = actions::push_vlan::ieee802_1ad();

        BOOST_TEST((ieee802_1Q == ieee802_1Q));
        BOOST_TEST((ieee802_1ad == ieee802_1ad));
        BOOST_TEST((ieee802_1Q != ieee802_1ad));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, push_vlan_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, push_vlan_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const action = actions::push_vlan::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((action == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // push_vlan_test
BOOST_AUTO_TEST_SUITE_END() // action_test
