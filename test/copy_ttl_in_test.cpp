#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/copy_ttl_in.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(copy_ttl_in_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = actions::copy_ttl_in{};

    BOOST_CHECK_EQUAL(sut.type(), OFPAT_COPY_TTL_IN);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(detail::ofp_action_header));
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::copy_ttl_in{};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::copy_ttl_in{};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::copy_ttl_in::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // copy_ttl_in_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
