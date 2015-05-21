#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/group.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <limits>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(group_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const id = std::uint32_t{0};
    auto const sut = actions::group{id};

    BOOST_CHECK_EQUAL(sut.type(), OFPAT_GROUP);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_action_group));
    BOOST_CHECK_EQUAL(sut.group_id(), id);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::group{34};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.group_id(), sut.group_id());
}

BOOST_AUTO_TEST_CASE(bad_group_id_test)
{
    auto const id = std::numeric_limits<std::uint32_t>::max();
    BOOST_CHECK_THROW(actions::group{id}, int);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::group{48};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::group::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.group_id(), sut.group_id());
}

BOOST_AUTO_TEST_SUITE_END() // group_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
