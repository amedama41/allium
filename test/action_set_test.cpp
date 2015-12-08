#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(action_set_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(default_constructor_test)
{
    auto const sut = action_set{};

    BOOST_CHECK_EQUAL(sut.length(), 0);
}

BOOST_AUTO_TEST_CASE(variadic_templete_constructor_test)
{
    auto const sut = action_set{
          actions::group{3}, actions::copy_ttl_in{}, actions::copy_ttl_out{}
        , actions::set_field{oxm_ipv4_src{0x7f000001}}
        , actions::set_field{oxm_eth_src{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
        , actions::output{3}
    };

    BOOST_CHECK_EQUAL(sut.length(), 8 + 8 + 8 + (4 + 4 + 4 + 7) / 8 * 8 + (4 + 4 + 6 + 7) / 8 * 8 + 16);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(variadic_templete_constructor_test_when_multiple_same_actions)
{
    auto const sut = action_set{
          actions::group{3}, actions::copy_ttl_in{}, actions::copy_ttl_out{}
        , actions::set_field{oxm_ipv4_src{0x7f000001}}
        , actions::set_field{oxm_ipv4_src{0x7f000002}}
        , actions::set_field{oxm_eth_src{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
        , actions::set_field{oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}}
        , actions::output{3}
        , actions::output{4}
    };

    BOOST_CHECK_EQUAL(sut.length(), 8 + 8 + 8 + (4 + 4 + 4 + 7) / 8 * 8 + (4 + 4 + 6 + 7) / 8 * 8 + 16);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = action_set{actions::set_queue{4}, actions::set_field{oxm_ip_dscp{4}}, actions::output{4}};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = action_set{actions::set_queue{4}, actions::set_field{oxm_ip_dscp{4}}, actions::output{4}};

    auto const copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.length(), 40);
    BOOST_CHECK_EQUAL(sut.length(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test


BOOST_AUTO_TEST_SUITE(assignment_test)

BOOST_AUTO_TEST_CASE(copy_assign_test)
{
    auto sut = action_set{actions::set_queue{4}, actions::set_field{oxm_ip_dscp{4}}, actions::output{4}};
    auto copy = action_set{actions::set_field{oxm_ip_dscp{4}}, actions::output{1}};

    copy = sut;

    BOOST_CHECK_EQUAL(copy.length(), sut.length());

    auto buffer = std::vector<std::uint8_t>{};
    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_set = action_set::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_set.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_assign_test)
{
    auto sut = action_set{actions::set_queue{4}, actions::set_field{oxm_ip_dscp{4}}, actions::output{4}};
    auto copy = action_set{actions::output{4}, actions::set_queue{4}, actions::set_field{oxm_ip_dscp{4}}, actions::output{1}};

    copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.length(), 40);
    BOOST_CHECK_EQUAL(sut.length(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // assignment_test


struct add_test_fixture
{
    action_set sut{};
};
BOOST_FIXTURE_TEST_SUITE(add_test, add_test_fixture)

BOOST_AUTO_TEST_CASE(add_one_action)
{
    auto const output = actions::output{32};

    sut.add(output);

    BOOST_CHECK_EQUAL(sut.length(), output.length());
}

BOOST_AUTO_TEST_CASE(add_multiple_action_one_by_one)
{
    auto set_ipv6_dst = actions::set_field{oxm_ipv6_dst{{{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x6, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0xd, 0xf}}}};
    auto set_queue = actions::set_queue{32};

    sut.add(set_ipv6_dst);
    sut.add(actions::set_queue{42});
    sut.add(std::move(set_queue));

    BOOST_CHECK_EQUAL(sut.length(), set_ipv6_dst.length() + set_queue.length());
}

#if 0 //TODO
BOOST_AUTO_TEST_CASE(add_ordered_action)
{
    auto const output = ordered_action{actions::output{32}};

    sut.add(output);

    BOOST_CHECK_EQUAL(sut.length(), output.length());
}
#endif

BOOST_AUTO_TEST_SUITE_END() // add_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = action_set{
          actions::group{3}, actions::copy_ttl_in{}, actions::copy_ttl_out{}
        , actions::set_field{oxm_ipv4_src{0x7f000001}}
        , actions::set_field{oxm_eth_src{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
        , actions::output{3}
    };

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_set = action_set::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_set.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // action_set_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
