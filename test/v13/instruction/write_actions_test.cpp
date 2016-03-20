#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction/write_actions.hpp>
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

struct write_actions_fixture
{
    instructions::write_actions sut{
          actions::set_ipv4_src{"192.168.10.1"_ipv4} // 16
        , actions::set_ipv4_dst{"127.0.0.1"_ipv4} // 16
        , actions::set_ip_proto{6} // 16
        , actions::copy_ttl_in{} // 8
        , actions::copy_ttl_out{} // 8
        , actions::output{0x12345678} // 16
    }; // 8 + 16 + 16 + 16 + 8 + 8 + 16 = 88
    std::vector<unsigned char> binary
        = "\x00\x03\x00\x58\x00\x00\x00\x00""\x00\x0c\x00\x08\x00\x00\x00\x00"
          "\x00\x0b\x00\x08\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x14\x01"
          "\x06\x00\x00\x00\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
          "\xc0\xa8\x0a\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x18\x04"
          "\x7f\x00\x00\x01\x00\x00\x00\x00""\x00\x00\x00\x10\x12\x34\x56\x78"
          "\xff\xff\x00\x00\x00\x00\x00\x00"
          ""_bin;
};

struct has_same_action_write_actions_fixture
{
    instructions::write_actions sut{v13::action_list{
          actions::output{0x12345678}
        , actions::output{0x10305070}
    }};
    std::vector<unsigned char> binary
        = "\x00\x03\x00\x28\x00\x00\x00\x00""\x00\x00\x00\x10\x12\x34\x56\x78"
          "\xff\xff\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x10\x10\x30\x50\x70"
          "\xff\xff\x00\x00\x00\x00\x00\x00"
          ""_bin;
};

}

BOOST_AUTO_TEST_SUITE(instruction_test)
BOOST_AUTO_TEST_SUITE(write_actions_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = instructions::write_actions;

        BOOST_TEST(sut::type() == v13::protocol::OFPIT_WRITE_ACTIONS);
    }

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = instructions::write_actions{};

        BOOST_TEST(sut.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(sut.actions().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_action_list_test)
    {
        auto const action_list = v13::action_list{
            actions::output{1}, actions::set_eth_type{0x0800}
        };

        auto const sut = instructions::write_actions{action_list};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 32);
        BOOST_TEST(sut.actions() == action_list);
    }

    BOOST_AUTO_TEST_CASE(construct_from_action_set_test)
    {
        auto const action_set = v13::action_set{
            actions::output{1}, actions::set_eth_type{0x0800}
        };

        auto const sut = instructions::write_actions{action_set};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 32);
        BOOST_TEST(sut.actions() == action_set.to_list());
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_action_test)
    {
        auto const group = actions::group{1};
        auto const action_list = v13::action_list{group};

        auto const sut = instructions::write_actions{group};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 8);
        BOOST_TEST(sut.actions() == action_list);
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_actions_test)
    {
        auto const copy_ttl_in = actions::copy_ttl_in{};
        auto const set_ipv4_src1 = actions::set_ipv4_src{"127.0.0.1"_ipv4};
        auto const set_ipv4_dst = actions::set_ipv4_dst{"127.0.0.2"_ipv4};
        auto const copy_ttl_out = actions::copy_ttl_out{};
        auto const set_ipv4_src2 = actions::set_ipv4_src{"172.16.1.1"_ipv4};
        auto const action_set = v13::action_set{
              copy_ttl_in, set_ipv4_src1, set_ipv4_dst
            , copy_ttl_in, copy_ttl_out, set_ipv4_src2
        };

        auto const sut = instructions::write_actions{
              copy_ttl_in, set_ipv4_src1, set_ipv4_dst
            , copy_ttl_in, copy_ttl_out, set_ipv4_src2
        };

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 48);
        BOOST_TEST(sut.actions() == action_set.to_list());
    }

    BOOST_AUTO_TEST_CASE(create_success_test)
    {
        auto const sut = instructions::write_actions::create(
                actions::output{1}, actions::group{2});

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_instruction_actions) + 16 + 8);
    }

    BOOST_AUTO_TEST_CASE(create_failure_test)
    {
        auto const action_list = v13::action_list{
            actions::output{1}, actions::output{2}
        };

        BOOST_CHECK_THROW(
                  instructions::write_actions::create(action_list)
                , std::runtime_error);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, write_actions_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST((copy.actions() == sut.actions()));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, write_actions_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST((copy.actions() == sut.actions()));
        BOOST_TEST(src.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(src.actions().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assign_test, write_actions_fixture)
    {
        auto copy = instructions::write_actions{};

        copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST((copy.actions() == sut.actions()));
    }

    BOOST_FIXTURE_TEST_CASE(move_assign_test, write_actions_fixture)
    {
        auto copy = instructions::write_actions{};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST((copy.actions() == sut.actions()));
        BOOST_TEST(src.length() == sizeof(detail::ofp_instruction_actions));
        BOOST_TEST(src.actions().empty());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = instructions::write_actions{
            actions::output{1}, actions::set_queue{2}
        };
        auto const diff_value = instructions::write_actions{
            actions::output{2}, actions::set_queue{1}
        };
        auto const diff_type = instructions::write_actions{
            actions::group{1}, actions::set_queue{2}
        };
        auto const empty = instructions::write_actions{};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != diff_type));
        BOOST_TEST((sut != empty));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, write_actions_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, write_actions_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const write_actions
            = instructions::write_actions::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((write_actions == sut));
    }

    BOOST_FIXTURE_TEST_CASE(
            decode_has_same_action_test, has_same_action_write_actions_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const write_actions
            = instructions::write_actions::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((write_actions == sut));
        BOOST_CHECK_THROW(
                  instructions::write_actions::validate(write_actions)
                , std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // write_actions_test
BOOST_AUTO_TEST_SUITE_END() // instruction_test
