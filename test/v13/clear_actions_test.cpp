#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/instruction/clear_actions.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(clear_actions_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = instructions::clear_actions{};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_CLEAR_ACTIONS);
    BOOST_CHECK_EQUAL(sut.length(), 8);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = instructions::clear_actions{};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = instructions::clear_actions{};

    auto const copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instructions::clear_actions{};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_instruction = instructions::clear_actions::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_instruction.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_instruction.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // clear_actions_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
