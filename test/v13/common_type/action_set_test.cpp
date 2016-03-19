#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <boost/test/unit_test.hpp>
#include <canard/network/protocol/openflow/v13/action_order.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;

namespace {

struct action_set_fixture
{
    v13::action_set sut{
          actions::output{0x12345678}
        , actions::set_ipv4_src{"192.168.10.31"_ipv4}
        , actions::push_vlan::ieee802_1Q()
        , actions::copy_ttl_out{}
    };
};

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(action_set_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::action_set{};

        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST(sut.length() == 0);
        BOOST_TEST((sut.begin() == sut.end()));

        BOOST_TEST(!sut.find<actions::output>());
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_action_test)
    {
        auto const output = actions::output{1};

        auto const sut = v13::action_set{output};

        BOOST_TEST(sut.length() == output.length());
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 1);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::output>()));
        BOOST_TEST((sut.find(v13::get_order(output)) != sut.end()));
        BOOST_TEST((sut.at(v13::get_order(output)) == output));
        BOOST_TEST((sut.at<actions::output>() == output));
        BOOST_TEST((sut.get<actions::output>() == output));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_actions)
    {
        auto const group = actions::group{3};
        auto const copy_ttl_in = actions::copy_ttl_in{};
        auto const copy_ttl_out = actions::copy_ttl_out{};
        auto const set_ipv4_src = actions::set_ipv4_src{"127.0.0.1"_ipv4};
        auto const set_eth_src
            = actions::set_eth_src{"\x01\x02\x03\x04\x05\x06"_mac};
        auto const output = actions::output{3};

        auto const sut = v13::action_set{
            group, copy_ttl_in, copy_ttl_out, set_ipv4_src, set_eth_src, output
        };

        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 6);
        BOOST_TEST(sut.length(), 8 + 8 + 8 + 16 + 16 + 16);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::group>()));
        BOOST_TEST((sut.get<actions::group>() == group));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::copy_ttl_in>()));
        BOOST_TEST((sut.get<actions::copy_ttl_in>() == copy_ttl_in));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::copy_ttl_out>()));
        BOOST_TEST((sut.get<actions::copy_ttl_out>() == copy_ttl_out));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_ipv4_src>()));
        BOOST_TEST((sut.get<actions::set_ipv4_src>() == set_ipv4_src));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_eth_src>()));
        BOOST_TEST((sut.get<actions::set_eth_src>() == set_eth_src));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::output>()));
        BOOST_TEST((sut.get<actions::output>() == output));
    }

    BOOST_AUTO_TEST_CASE(construct_from_same_actions_test)
    {
        auto const group = actions::group{3}; // 8
        auto const set_ipv4_src1
            = actions::set_ipv4_src{"127.0.0.1"_ipv4}; // 16
        auto const set_ipv4_src2
            = actions::set_ipv4_src{"127.0.0.2"_ipv4}; // 16
        auto const output1 = actions::output{3}; // 16
        auto const output2 = actions::output{4}; // 16

        auto const sut = v13::action_set{
              group, set_ipv4_src1, output1, output2, set_ipv4_src2
        };

        BOOST_TEST(sut.size() == 3);
        BOOST_TEST(sut.length() == 8 + 16 + 16);

        BOOST_TEST_REQUIRE(bool(sut.find<actions::group>()));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_ipv4_src>()));
        BOOST_TEST((sut.get<actions::set_ipv4_src>() == set_ipv4_src1));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::output>()));
        BOOST_TEST((sut.get<actions::output>() == output1));
    }

    BOOST_AUTO_TEST_CASE(construct_from_any_action_test)
    {
        auto const group1 = v13::any_action{actions::group{3}};
        auto const group2 = actions::group{4};
        auto const copy_ttl_in = actions::copy_ttl_in{};
        auto const set_ipv4_src
            = v13::any_action{actions::set_ipv4_src{"127.0.0.1"_ipv4}};

        auto const sut = v13::action_set{
            group1, group2, copy_ttl_in, set_ipv4_src
        };

        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 3);
        BOOST_TEST(sut.length(), 8 + 8 + 16);
        BOOST_TEST((sut.begin() != sut.end()));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::group>()));
        BOOST_TEST((sut.get<actions::group>() == group1));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::copy_ttl_in>()));
        BOOST_TEST((sut.get<actions::copy_ttl_in>() == copy_ttl_in));

        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_ipv4_src>()));
        BOOST_TEST((sut.get<actions::set_ipv4_src>() == set_ipv4_src));
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = v13::action_set{
              actions::pop_mpls::ipv4()
            , actions::set_eth_type{0x0800}
            , actions::set_sctp_dst{6653}
        };
        auto const diff_order = v13::action_set{
              actions::set_sctp_dst{6653}
            , actions::set_eth_type{0x0800}
            , actions::pop_mpls::ipv4()
        };
        auto const diff_types = v13::action_set{
              actions::pop_pbb{}
            , actions::set_eth_type{0x0800}
            , actions::set_sctp_src{6653}
        };
        auto const diff_value = v13::action_set{
              actions::pop_mpls::unicast()
            , actions::set_eth_type{0x8100}
            , actions::set_sctp_dst{6633}
        };
        auto const less_size = v13::action_set{
              actions::pop_mpls::ipv4()
            , actions::set_eth_type{0x0800}
        };
        auto const grater_size = v13::action_set{
              actions::pop_mpls::ipv4()
            , actions::set_eth_type{0x0800}
            , actions::set_sctp_dst{6653}
            , actions::output::to_controller()
        };
        auto const empty = v13::action_set{};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == diff_order));
        BOOST_TEST((sut != diff_types));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != less_size));
        BOOST_TEST((sut != grater_size));
        BOOST_TEST((sut != empty));
    }

    BOOST_FIXTURE_TEST_CASE(insert_new_action_test, action_set_fixture)
    {
        auto const set_eth_type = actions::set_eth_type{0x0800};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(set_eth_type);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == set_eth_type));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + set_eth_type.length());
        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_eth_type>()));
        BOOST_TEST((sut.get<actions::set_eth_type>() == set_eth_type));
    }

    BOOST_FIXTURE_TEST_CASE(insert_existing_action_test, action_set_fixture)
    {
        auto const output = actions::output::to_controller();
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(output);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first != output));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(bool(sut.find<actions::output>()));
        BOOST_TEST((sut.get<actions::output>() != output));
    }

    BOOST_FIXTURE_TEST_CASE(insert_any_action_test, action_set_fixture)
    {
        auto const action = v13::any_action{actions::pop_vlan{}};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(action);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == action));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + action.length());
        BOOST_TEST_REQUIRE((sut.find(v13::get_order(action)) != sut.end()));
        BOOST_TEST((sut.at(v13::get_order(action)) == action));
    }

    BOOST_FIXTURE_TEST_CASE(assign_new_action_test, action_set_fixture)
    {
        auto const group = actions::group{0x01020304};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.assign(group);

        BOOST_TEST(!result.second);
        BOOST_TEST((result.first == sut.end()));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(!sut.find<actions::group>());
    }

    BOOST_FIXTURE_TEST_CASE(assign_existing_action_test, action_set_fixture)
    {
        auto const set_ipv4_src = actions::set_ipv4_src{"127.0.0.1"_ipv4};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.assign(set_ipv4_src);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == set_ipv4_src));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(bool(sut.find<actions::set_ipv4_src>()));
        BOOST_TEST((sut.get<actions::set_ipv4_src>() == set_ipv4_src));
    }

    BOOST_FIXTURE_TEST_CASE(assign_any_action_test, action_set_fixture)
    {
        auto const action = v13::any_action{actions::output{0}};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.assign(action);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == action));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE((sut.find(v13::get_order(action)) != sut.end()));
        BOOST_TEST((sut.at(v13::get_order(action)) == action));
    }

    BOOST_FIXTURE_TEST_CASE(
            insert_or_assign_new_action_test, action_set_fixture)
    {
        auto const decrement_nw_ttl = actions::decrement_nw_ttl{};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert_or_assign(decrement_nw_ttl);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == decrement_nw_ttl));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + decrement_nw_ttl.length());
        BOOST_TEST_REQUIRE(bool(sut.find<actions::decrement_nw_ttl>()));
        BOOST_TEST((sut.get<actions::decrement_nw_ttl>() == decrement_nw_ttl));
    }

    BOOST_FIXTURE_TEST_CASE(
            insert_or_assign_existing_action_test, action_set_fixture)
    {
        auto const push_vlan = actions::push_vlan::ieee802_1ad();
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert_or_assign(push_vlan);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first == push_vlan));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(bool(sut.find<actions::push_vlan>()));
        BOOST_TEST((sut.get<actions::push_vlan>() == push_vlan));
    }

    BOOST_FIXTURE_TEST_CASE(
            insert_or_assign_any_action_test, action_set_fixture)
    {
        auto const action = v13::any_action{actions::output{1}};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert_or_assign(action);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first == action));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE((sut.find(v13::get_order(action)) != sut.end()));
        BOOST_TEST((sut.at(v13::get_order(action)) == action));
    }

    BOOST_FIXTURE_TEST_CASE(erase_new_field_test, action_set_fixture)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.erase<actions::set_queue>();

        BOOST_TEST(result == 0);
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST(!sut.find<actions::set_queue>());
    }

    BOOST_FIXTURE_TEST_CASE(erase_existing_field_test, action_set_fixture)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_field_length
            = sut.at(v13::action_order<actions::copy_ttl_out>::value).length();

        auto const result = sut.erase<actions::copy_ttl_out>();

        BOOST_TEST(result == 1);
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_field_length);
        BOOST_TEST(!sut.find<actions::copy_ttl_out>());
    }

    BOOST_FIXTURE_TEST_CASE(erase_by_iterator_test, action_set_fixture)
    {
        auto it = std::prev(sut.end(), 2);
        auto const erased_action_order = v13::get_order(*it);
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_field_length = it->length();

        auto const result = sut.erase(it);

        BOOST_TEST((result == std::prev(sut.end(), 1)));
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_field_length);
        BOOST_TEST((sut.find(erased_action_order) == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(clear_test, action_set_fixture)
    {
        sut.clear();

        BOOST_TEST(sut.length() == 0);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(swap_test, action_set_fixture)
    {
        auto other = v13::action_set{};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        sut.swap(other);

        BOOST_TEST(sut.empty());
        BOOST_TEST(other.size() == before_size);
        BOOST_TEST(other.length() == before_length);
    }

BOOST_AUTO_TEST_SUITE_END() // action_set_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
