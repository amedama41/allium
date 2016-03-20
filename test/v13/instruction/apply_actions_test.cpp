#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction/apply_actions.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;
namespace instructions = v13::instructions;
namespace detail = v13::v13_detail;

namespace {

struct apply_actions_fixture
{
    instructions::apply_actions sut{
          actions::push_vlan{0x8100}
        , actions::set_vlan_vid{v13::protocol::OFPVID_PRESENT | 0x0123}
        , actions::output{0x12345678}
        , actions::set_eth_dst{"\x66\x55\x44\x33\x22\x11"_mac}
        , actions::output{v13::protocol::OFPP_MAX}
    }; // 8 + 8 + 16 + 16 + 16 + 16
    std::vector<unsigned char> binary
        = "\x00\x04\x00\x50\x00\x00\x00\x00""\x00\x11\x00\x08\x81\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x0c\x02""\x11\x23\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x10\x12\x34\x56\x78""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x66\x55\x44\x33\x22\x11\x00\x00"
          "\x00\x00\x00\x10\xff\xff\xff\x00""\xff\xff\x00\x00\x00\x00\x00\x00"
          ""_bin;
};

}

BOOST_AUTO_TEST_SUITE(instruction_test)
BOOST_AUTO_TEST_SUITE(apply_actions_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = instructions::apply_actions;

        BOOST_TEST(sut::type() == v13::protocol::OFPIT_APPLY_ACTIONS);
    }

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = instructions::apply_actions{};

        BOOST_TEST(sut.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(sut.actions().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_action_list_test)
    {
        auto const action_list = v13::action_list{
            actions::set_queue{1}, actions::set_vlan_pcp{2}
        };

        auto const sut = instructions::apply_actions{action_list};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 24);
        BOOST_TEST((sut.actions() == action_list));
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_action_test)
    {
        auto const group = actions::group{1};
        auto const action_list = v13::action_list{group};

        auto const sut = instructions::apply_actions{group};

        BOOST_TEST(sut.length() == sizeof(detail::ofp_instruction_actions) + 8);
        BOOST_TEST((sut.actions() == action_list));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_actions_test)
    {
        auto const copy_ttl_in = actions::copy_ttl_in{};
        auto const set_ipv4_src = actions::set_ipv4_src{"127.0.0.1"_ipv4};
        auto const set_vlan_vid1
            = actions::set_vlan_vid{v13::protocol::OFPVID_PRESENT | 1};
        auto const push_vlan = actions::push_vlan{0x8100};
        auto const set_vlan_vid2
            = actions::set_vlan_vid{v13::protocol::OFPVID_PRESENT | 1024};
        auto const output = actions::output{4};
        auto const action_list = v13::action_list{
              copy_ttl_in, set_ipv4_src, set_vlan_vid1
            , push_vlan, set_vlan_vid2, output
        };

        auto const sut = instructions::apply_actions{
              copy_ttl_in, set_ipv4_src, set_vlan_vid1
            , push_vlan, set_vlan_vid2, output
        };

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 80);
        BOOST_TEST((sut.actions() == action_list));
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const sut = instructions::apply_actions::create(
                  actions::set_vlan_vid{v13::protocol::OFPVID_PRESENT | 0x0123}
                , actions::output{1});

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 16 + 16);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, apply_actions_fixture)
    {
        auto const copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, apply_actions_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(src.actions().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assign_test, apply_actions_fixture)
    {
        auto copy = instructions::apply_actions{};

        copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_assign_test, apply_actions_fixture)
    {
        auto copy = instructions::apply_actions{};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(src.actions().empty());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = instructions::apply_actions{
            actions::output{1}, actions::set_queue{2}
        };
        auto const diff_value = instructions::apply_actions{
            actions::output{2}, actions::set_queue{1}
        };
        auto const diff_type = instructions::apply_actions{
            actions::group{1}, actions::set_queue{2}
        };
        auto const empty = instructions::apply_actions{};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != diff_type));
        BOOST_TEST((sut != empty));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, apply_actions_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, apply_actions_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const apply_actions
            = instructions::apply_actions::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((apply_actions == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // apply_actions_test
BOOST_AUTO_TEST_SUITE_END() // instruction_test
