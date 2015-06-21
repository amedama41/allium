#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/set_queue.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <limits>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(set_queue_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = actions::set_queue{0};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPAT_SET_QUEUE);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_action_set_queue));
    BOOST_CHECK_EQUAL(sut.queue_id(), 0);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::set_queue{std::numeric_limits<std::uint32_t>::max() - 1};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.queue_id(), sut.queue_id());
}

BOOST_AUTO_TEST_CASE(queue_id_is_OFPQ_ALL)
{
    BOOST_CHECK_THROW(actions::set_queue{protocol::OFPQ_ALL}, int);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::set_queue{255};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::set_queue::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.queue_id(), sut.queue_id());
}

BOOST_AUTO_TEST_SUITE_END() // set_queue_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
