#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;

using boost::asio::ip::address_v4;
using canard::mac_address;

namespace {

struct action_list_fixture
{
    v13::action_list sut{
          actions::group{0x12345678}
        , actions::copy_ttl_in{}
        , actions::copy_ttl_out{}
        , actions::set_ipv4_src{"127.0.0.1"_ipv4}
        , actions::set_eth_src{"\x01\x02\x03\x04\x05\x06"_mac}
        , actions::output{0x87654321}
    };
    std::vector<std::uint8_t> binary
        = "\x00\x16\x00\x08\x12\x34\x56\x78""\x00\x0c\x00\x08\x00\x00\x00\x00"
          "\x00\x0b\x00\x08\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
          "\x7f\x00\x00\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x08\x06"
          "\x01\x02\x03\x04\x05\x06\x00\x00""\x00\x00\x00\x10\x87\x65\x43\x21"
          "\xff\xff\x00\x00\x00\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(action_list_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::action_list{};

        BOOST_TEST(sut.length() == 0);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_action_test)
    {
        auto const output = actions::output{1};

        auto const sut = v13::action_list{output};

        BOOST_TEST(sut.length() == output.length());
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 1);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST((sut.at(0) == output));
        BOOST_TEST((sut[0] == output));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_actions_test)
    {
        auto const output = actions::output{4}; // 16
        auto const set_ipv4_src = actions::set_ipv4_src{"127.0.0.1"_ipv4}; // 16
        auto const copy_ttl_in = actions::copy_ttl_in{}; // 8
        auto const push_vlan = actions::push_vlan{0x8100}; // 8
        auto const set_tcp_dst = actions::set_tcp_dst{6653}; // 16
        auto const set_queue = actions::set_queue{4}; // 8

        auto const sut = v13::action_list{
            output, set_ipv4_src, copy_ttl_in, push_vlan, set_tcp_dst, set_queue
        };

        BOOST_TEST(sut.length() == 72);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 6);
        BOOST_TEST((sut.begin() != sut.end()));
        BOOST_TEST((sut.at(0) == output));
        BOOST_TEST((sut[0] == output));
        BOOST_TEST((sut.at(1) == set_ipv4_src));
        BOOST_TEST((sut[1] == set_ipv4_src));
        BOOST_TEST((sut.at(2) == copy_ttl_in));
        BOOST_TEST((sut[2] == copy_ttl_in));
        BOOST_TEST((sut.at(3) == push_vlan));
        BOOST_TEST((sut[3] == push_vlan));
        BOOST_TEST((sut.at(4) == set_tcp_dst));
        BOOST_TEST((sut[4] == set_tcp_dst));
        BOOST_TEST((sut.at(5) == set_queue));
        BOOST_TEST((sut[5] == set_queue));
    }

    BOOST_AUTO_TEST_CASE(construct_from_same_actions_test)
    {
        auto const output1 = actions::output{4}; // 16
        auto const group = actions::group{1}; // 8
        auto const output2 = actions::output{5}; // 16

        auto const sut = v13::action_list{output1, group, output2};

        BOOST_TEST(sut.length() == 40);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 3);
        BOOST_TEST((sut.at(0) == output1));
        BOOST_TEST((sut.at(1) == group));
        BOOST_TEST((sut.at(2) == output2));
    }

    BOOST_AUTO_TEST_SUITE(equality_test)

        BOOST_AUTO_TEST_CASE(different_action_value_test)
        {
            auto const sut1
                = v13::action_list{actions::output{1}, actions::group{1}};
            auto const sut2
                = v13::action_list{actions::output{1}, actions::group{2}};
            auto const sut3
                = v13::action_list{actions::output{2}, actions::group{1}};
            auto const sut4
                = v13::action_list{actions::output{2}, actions::group{2}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
            BOOST_TEST((sut1 != sut4));
        }

        BOOST_AUTO_TEST_CASE(different_size_test)
        {
            auto const sut1
                = v13::action_list{actions::push_pbb{}, actions::set_queue{1}};
            auto const sut2
                = v13::action_list{actions::push_pbb{}};
            auto const sut3 = v13::action_list{
                actions::push_pbb{}, actions::set_queue{1}, actions::push_pbb{}
            };

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
        }

        BOOST_AUTO_TEST_CASE(different_order_test)
        {
            auto const output = actions::output{1};
            auto const set_eth_type = actions::set_eth_type{0x8100};
            auto const pop_pbb = actions::pop_pbb{};
            auto const sut1 = v13::action_list{output, set_eth_type, pop_pbb};
            auto const sut2 = v13::action_list{output, pop_pbb, set_eth_type};
            auto const sut3 = v13::action_list{set_eth_type, output, pop_pbb};
            auto const sut4 = v13::action_list{set_eth_type, pop_pbb, output};
            auto const sut5 = v13::action_list{pop_pbb, output, set_eth_type};
            auto const sut6 = v13::action_list{pop_pbb, set_eth_type, output};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
            BOOST_TEST((sut1 != sut3));
            BOOST_TEST((sut1 != sut4));
            BOOST_TEST((sut1 != sut5));
            BOOST_TEST((sut1 != sut6));
        }

        BOOST_AUTO_TEST_CASE(empty_action_list_test)
        {
            auto const sut1 = v13::action_list{};
            auto const sut2 = v13::action_list{actions::pop_vlan{}};

            BOOST_TEST((sut1 == sut1));
            BOOST_TEST((sut1 != sut2));
        }

    BOOST_AUTO_TEST_SUITE_END() // equality_test

    BOOST_FIXTURE_TEST_CASE(encode_test, action_list_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, action_list_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const match_set = v13::action_list::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((match_set == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // action_list_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
