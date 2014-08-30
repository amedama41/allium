#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE push_pbb_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/action/push_pbb.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(push_pbb_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = actions::push_pbb{};

    BOOST_CHECK_EQUAL(sut.type(), OFPAT_PUSH_PBB);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(detail::ofp_action_push));
    BOOST_CHECK_EQUAL(sut.ethertype(), 0x88e7);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::push_pbb{};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.ethertype(), sut.ethertype());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::push_pbb{};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::push_pbb::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.ethertype(), sut.ethertype());
}

BOOST_AUTO_TEST_SUITE_END() // push_pbb_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
