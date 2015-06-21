#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/group_mod.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <utility>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(group_mod_test)

BOOST_AUTO_TEST_SUITE(group_mod_add_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const group_id = 0;
        auto const sut = group_mod_add{group_id, protocol::OFPGT_INDIRECT, {bucket{actions::output{4}}}};

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_GROUP_MOD);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_group_mod) + sizeof(v13_detail::ofp_bucket) + sizeof(v13_detail::ofp_action_output));
        BOOST_CHECK_EQUAL(sut.command(), protocol::OFPGC_ADD);
        BOOST_CHECK_EQUAL(sut.group_id(), group_id);
        BOOST_CHECK_EQUAL(sut.group_type(), protocol::OFPGT_INDIRECT);
        BOOST_CHECK_EQUAL(sut.buckets().size(), 1);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = group_mod_add{protocol::OFPG_MAX, protocol::OFPGT_SELECT, {
              bucket{2, {actions::pop_vlan{}, actions::output{1}}} // 16 + 8 + 16 = 40
            , bucket{1, {actions::set_field{oxm_vlan_vid{4}}, actions::output{2}}} // 16 + 16 + 16 = 48
        }};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sizeof(v13_detail::ofp_group_mod) + 40 + 48);
        BOOST_CHECK_EQUAL(copy.command(), sut.command());
        BOOST_CHECK_EQUAL(copy.group_id(), sut.group_id());
        BOOST_CHECK_EQUAL(copy.group_type(), sut.group_type());
        BOOST_CHECK_EQUAL(copy.buckets().size(), 2);

        BOOST_CHECK_EQUAL(sut.buckets().size(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture {
    group_mod_add const sut{1, protocol::OFPGT_ALL, {
          bucket{{actions::push_vlan{0x8100}, actions::set_field{oxm_vlan_vid{4095}}, actions::output{1}}}
        , bucket{{actions::set_field{oxm_ipv4_src{0x7f000001}}, actions::set_field{oxm_ipv4_dst{0x7f000002}}, actions::output{2}}}
    }};
    std::vector<std::uint8_t> buffer{};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        sut.encode(buffer);
        auto it = buffer.begin();

        auto const decoded_message = group_mod_add::decode(it, buffer.end());
        BOOST_CHECK(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
        BOOST_CHECK_EQUAL(decoded_message.command(), sut.command());
        BOOST_CHECK_EQUAL(decoded_message.group_id(), sut.group_id());
        BOOST_CHECK_EQUAL(decoded_message.group_type(), sut.group_type());
        BOOST_CHECK_EQUAL(decoded_message.buckets().size(), sut.buckets().size());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // group_mod_add_test


BOOST_AUTO_TEST_SUITE(group_mod_delete_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const group_id = protocol::OFPG_ANY;
        auto const sut = group_mod_delete{group_id};

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_GROUP_MOD);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_group_mod));
        BOOST_CHECK_EQUAL(sut.command(), protocol::OFPGC_DELETE);
        BOOST_CHECK_EQUAL(sut.group_id(), group_id);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = group_mod_delete{0};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sizeof(v13_detail::ofp_group_mod));
        BOOST_CHECK_EQUAL(copy.command(), sut.command());
        BOOST_CHECK_EQUAL(copy.group_id(), sut.group_id());
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture {
    group_mod_delete const sut{protocol::OFPG_MAX};
    std::vector<std::uint8_t> buffer;
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        sut.encode(buffer);
        auto it = buffer.begin();

        auto const decoded_message = group_mod_delete::decode(it, buffer.end());
        BOOST_CHECK(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
        BOOST_CHECK_EQUAL(decoded_message.command(), sut.command());
        BOOST_CHECK_EQUAL(decoded_message.group_id(), sut.group_id());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // group_mod_delete_test

BOOST_AUTO_TEST_SUITE_END() // group_mod_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
