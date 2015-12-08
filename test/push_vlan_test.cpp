#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/action/push_vlan.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(push_vlan_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = actions::push_vlan{0x8100};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPAT_PUSH_VLAN);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_action_push));
    BOOST_CHECK_EQUAL(sut.ethertype(), 0x8100);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::push_vlan{0x88a8};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.ethertype(), sut.ethertype());
}

BOOST_AUTO_TEST_CASE(bad_ethertype)
{
    BOOST_CHECK_THROW(actions::push_vlan{0x80ff}, int);
    BOOST_CHECK_THROW(actions::push_vlan{0x8101}, int);
    BOOST_CHECK_THROW(actions::push_vlan{0x88a7}, int);
    BOOST_CHECK_THROW(actions::push_vlan{0x88a9}, int);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::push_vlan{0x88a8};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::push_vlan::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.ethertype(), sut.ethertype());
}

BOOST_AUTO_TEST_SUITE_END() // push_vlan_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
