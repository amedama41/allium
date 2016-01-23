#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/instruction/apply_actions.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(apply_actions_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(default_constructor_test)
{
    auto const sut = instructions::apply_actions{};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8);
}

BOOST_AUTO_TEST_CASE(variadic_template_constructor_test)
{
    auto lvalue_action = actions::copy_ttl_in{};
    auto const const_lvalue_action = actions::set_field{oxm_ipv4_src{0x7f000001}};
    auto rvalue_action = actions::set_field{oxm_vlan_vid{1}};
    auto const sut = instructions::apply_actions{
          lvalue_action, const_lvalue_action, std::move(rvalue_action)
        , actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
    };

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + 80);
}

BOOST_AUTO_TEST_SUITE(initialize_by_action_list)

BOOST_AUTO_TEST_CASE(constructed_by_lvalue)
{
    auto list = action_list{
          actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
        , actions::pop_vlan{}, actions::output{protocol::OFPP_MAX}
    };

    auto const sut = instructions::apply_actions{list};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + list.length());
}

BOOST_AUTO_TEST_CASE(constructed_by_const_lvalue)
{
    auto const list = action_list{
          actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
        , actions::set_field{oxm_eth_dst{{{0x66, 0x55, 0x44, 0x33, 0x22, 0x11}}}}, actions::output{protocol::OFPP_MAX}
    };

    auto const sut = instructions::apply_actions{list};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + list.length());
}

BOOST_AUTO_TEST_CASE(constructed_by_xvalue)
{
    auto list = action_list{
          actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
        , actions::set_field{oxm_eth_dst{{{0x66, 0x55, 0x44, 0x33, 0x22, 0x11}}}}, actions::output{protocol::OFPP_MAX}
    };
    auto const list_lenght = list.length();

    auto const sut = instructions::apply_actions{std::move(list)};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + list_lenght);
    BOOST_CHECK_EQUAL(list.length(), 0);
}

BOOST_AUTO_TEST_CASE(constructed_by_prvalue)
{

    auto const sut = instructions::apply_actions{
        action_list{
              actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
            , actions::set_field{oxm_eth_dst{{{0x66, 0x55, 0x44, 0x33, 0x22, 0x11}}}}, actions::output{protocol::OFPP_MAX}
        }
    };

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_APPLY_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 80);
}

BOOST_AUTO_TEST_SUITE_END() // initialize_by_action_list

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = instructions::apply_actions{
        actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
    };

    auto copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = instructions::apply_actions{
        actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{1024}}, actions::output{4}
    };
    auto const expected_length = sut.length();

    auto copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), expected_length);

    // TODO
    auto buffer = std::vector<std::uint8_t>{};
    BOOST_CHECK_EQUAL(sut.encode(buffer).size(), 8);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instructions::apply_actions{
        actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}
    };

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_instruction = instructions::apply_actions::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_instruction.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_instruction.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // apply_actions_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
