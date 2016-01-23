#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(action_list_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = action_list{};

        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

    BOOST_AUTO_TEST_CASE(variadic_template_constructor_test)
    {
        auto const sut = action_list{
              actions::output{4}, actions::set_field{oxm_ipv4_src{0x7f000001}}
            , actions::copy_ttl_in{}, actions::push_vlan{0x8100}
            , actions::set_field{oxm_tcp_dst{6653}}
            , actions::set_queue{4}, actions::output{3}
        };

        BOOST_CHECK_EQUAL(sut.length(), 88);
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_test)
    {
        auto sut = action_list{
              actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::output::to_controller()
        };

        auto copy = sut;

        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = action_list{
              actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::output::to_controller()
        };

        auto copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.length(), 32);
        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(assignment_test)

    BOOST_AUTO_TEST_CASE(copy_assign_test)
    {
        auto sut = action_list{
              actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::output::to_controller()
        };

        auto copy = action_list{
              actions::set_field{oxm_eth_type{0x0800}}
            , actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::output{4}
        };

        copy = sut;

        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_assign_test)
    {
        auto sut = action_list{
              actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::output::to_controller()
        };
        auto const expected_length = sut.length();

        auto copy = action_list{
              actions::set_field{oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
            , actions::set_field{oxm_eth_type{0x0800}}
            , actions::output{4}
        };

        copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.length(), expected_length);
        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // assignment_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = action_list{
          actions::group{3}, actions::copy_ttl_in{}, actions::copy_ttl_out{}
        , actions::set_field{oxm_ipv4_src{0x7f000001}}
        , actions::set_field{oxm_eth_src{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}}
        , actions::output{3}
    };

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_set = action_list::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_set.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // action_list_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
