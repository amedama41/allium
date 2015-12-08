#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_actions.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(default_constructor_test)
{
}

BOOST_AUTO_TEST_CASE(variadic_templete_constructor_test)
{
    auto lvalue_action = actions::copy_ttl_in{};
    auto const const_lvalue_action = actions::set_field{oxm_ipv4_src{0x7f000001}};
    auto rvalue_action = actions::set_field{oxm_ipv4_dst{0x7f000002}};
    auto sut = instructions::write_actions{
          lvalue_action, const_lvalue_action, std::move(rvalue_action)
        , actions::copy_ttl_in{}, actions::copy_ttl_out{}
        , actions::set_field{oxm_ipv4_src{0x0100007f}}
    };

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_WRITE_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + 48);
}

BOOST_AUTO_TEST_SUITE(initialize_by_action_set)

BOOST_AUTO_TEST_CASE(constructed_by_lvalue)
{
    auto set = action_set{actions::copy_ttl_in{}, actions::set_field{oxm_udp_dst{3344}}};

    auto const sut = instructions::write_actions{set};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_WRITE_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + set.length());
}

BOOST_AUTO_TEST_CASE(constructed_by_const_lvalue)
{
    auto const set = action_set{actions::copy_ttl_in{}, actions::set_field{oxm_arp_op{1}}};

    auto const sut = instructions::write_actions{set};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_WRITE_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + set.length());
}

BOOST_AUTO_TEST_CASE(constructed_by_xvalue)
{
    auto set = action_set{actions::copy_ttl_in{}, actions::set_field{oxm_udp_dst{3344}}};
    auto const set_length = set.length();

    auto const sut = instructions::write_actions{std::move(set)};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_WRITE_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + set_length);
    BOOST_CHECK_EQUAL(set.length(), 0);
}

BOOST_AUTO_TEST_CASE(constructed_by_prvalue)
{
    auto const sut = instructions::write_actions{
        action_set{actions::copy_ttl_in{}, actions::set_field{oxm_udp_dst{3344}}}
    };

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_WRITE_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8 + 24);
}

BOOST_AUTO_TEST_SUITE_END() // initialize_by_action_set

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto const set = action_set{actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}};
    auto sut = instructions::write_actions{set};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = instructions::write_actions{
        action_set{actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}}
    };

    auto const copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());

    // TODO
    auto buffer = std::vector<std::uint8_t>{};
    BOOST_CHECK_EQUAL(sut.encode(buffer).size(), sizeof(v13_detail::ofp_instruction_actions));
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test


BOOST_AUTO_TEST_SUITE(assignment_test)

BOOST_AUTO_TEST_CASE(copy_assign_test)
{
    auto const set = action_set{actions::set_field{oxm_ipv6_dst{{{}}}}, actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}};
    auto sut = instructions::write_actions{set};
    auto copy = instructions::write_actions{actions::set_queue{4}, actions::copy_ttl_in{}, actions::set_field{oxm_arp_op{1}}};

    copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_assign_test)
{
    auto const set = action_set{actions::set_field{oxm_ipv6_dst{{{}}}}, actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}};
    auto sut = instructions::write_actions{set};
    auto copy = instructions::write_actions{actions::copy_ttl_in{}, actions::set_field{oxm_arp_op{1}}};

    copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());

    // TODO
    auto buffer = std::vector<std::uint8_t>{};
    BOOST_CHECK_EQUAL(sut.encode(buffer).size(), sizeof(v13_detail::ofp_instruction_actions));
}

BOOST_AUTO_TEST_SUITE_END() // assignment_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instructions::write_actions{
        action_set{actions::copy_ttl_in{}, actions::set_field{oxm_ip_proto{1}}, actions::output{4}}
    };

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_instruction = instructions::write_actions::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_instruction.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_instruction.length(), sut.length());
}

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
