#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/flow_delete.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

namespace  {

struct flow_entry_fixture {
    std::array<std::uint8_t, 6> eth_dst = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
    std::array<std::uint8_t, 6> eth_src = {{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}};
    v13::flow_entry entry = {
          v13::oxm_match_set{
              v13::oxm_in_port{4}
            , v13::oxm_eth_dst{eth_dst}
            , v13::oxm_eth_src{eth_src}
          } // 4 + 8 + 10 + 10 = 32
        , proto::OFP_DEFAULT_PRIORITY
        , 0xf1f2f3f4f5f6f7f8
        , v13::instruction_set{}
    };
};

struct flow_delete_fixture : flow_entry_fixture
{
    v13::messages::flow_delete sut = {
          entry.match()
        , proto::OFPTT_ALL
        , v13::cookie_mask{0xf1f2f3f4f5f6f7f8, 0x0f0f0f0f0f0f0f0f}
        , proto::OFPP_ANY, 33
        , 0xff56ff78
    };

    std::vector<std::uint8_t> bin_flow_delete
        = "\x04\x0e\x00\x50\xff\x56\xff\x78" "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f" "\xff\x03\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\xff\xff\xff\xff" "\x00\x00\x00\x21\x00\x00\x00\x00"

          "\x00\x01\x00\x20\x80\x00\x00\x04" "\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00" "\x08\x06\x11\x12\x13\x14\x15\x16"_bin
        ;
};

struct flow_delete_strict_fixture : flow_entry_fixture
{
    v13::messages::flow_delete_strict sut = {
          entry
        , 0x8f
        , proto::OFPP_CONTROLLER, proto::OFPG_MAX
        , 0xff56ff78
    };

    std::vector<std::uint8_t> bin_flow_delete_strict
        = "\x04\x0e\x00\x50\xff\x56\xff\x78" "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\xff\xff\xff\xff\xff\xff\xff\xff" "\x8f\x04\x00\x00\x00\x00\x80\x00"
          "\x00\x00\x00\x00\xff\xff\xff\xfd" "\xff\xff\xff\x00\x00\x00\x00\x00"

          "\x00\x01\x00\x20\x80\x00\x00\x04" "\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00" "\x08\x06\x11\x12\x13\x14\x15\x16"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(flow_delete_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_table_id_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};

        auto const sut = v13::messages::flow_delete{
            entry.match(), table_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_out_port_test)
    {
        auto const table_id = std::uint8_t{proto::OFPTT_ALL};
        auto const out_port = std::uint32_t{1};
        auto const out_group = std::uint32_t{0};

        auto const sut = v13::messages::flow_delete{
            v13::oxm_match_set{}, table_id, out_port, out_group
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == 4);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == out_port);
        BOOST_TEST(sut.out_group() == out_group);
    }

    BOOST_AUTO_TEST_CASE(construct_from_cookie_mask_test)
    {
        auto const table_id = std::uint8_t{254};
        auto const cookie_mask = v13::cookie_mask{0xff, 0xffff};

        auto const sut = v13::messages::flow_delete{
            v13::oxm_match_set{}, table_id, cookie_mask
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == 4);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_cookie_mask_and_port_test)
    {
        auto const table_id = std::uint8_t{0};
        auto const cookie_mask = v13::cookie_mask{0xff, 0xffff};
        auto const out_port = std::uint32_t{proto::OFPP_MAX};
        auto const out_group = std::uint32_t{proto::OFPG_MAX};

        auto const sut = v13::messages::flow_delete{
            v13::oxm_match_set{}, table_id, cookie_mask, out_port, out_group
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == 4);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.out_port() == out_port);
        BOOST_TEST(sut.out_group() == out_group);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_delete_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_delete_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_delete_fixture)
    {
        auto copy = v13::messages::flow_delete{v13::oxm_match_set{}, 0};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_delete_fixture)
    {
        auto copy = v13::messages::flow_delete{v13::oxm_match_set{}, 0};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_delete_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_delete, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_delete_fixture)
    {
        auto it = bin_flow_delete.begin();
        auto const it_end = bin_flow_delete.end();

        auto const flow_delete = v13::messages::flow_delete::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_delete.version() == sut.version());
        BOOST_TEST(flow_delete.type() == sut.type());
        BOOST_TEST(flow_delete.length() == sut.length());
        BOOST_TEST(flow_delete.xid() == sut.xid());
        BOOST_TEST(flow_delete.match().length() == sut.match().length());
        BOOST_TEST(flow_delete.table_id() == sut.table_id());
        BOOST_TEST(flow_delete.cookie() == sut.cookie());
        BOOST_TEST(flow_delete.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(flow_delete.out_port() == sut.out_port());
        BOOST_TEST(flow_delete.out_group() == sut.out_group());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_delete_test


BOOST_AUTO_TEST_SUITE(flow_delete_strict_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};

        auto const sut = v13::messages::flow_delete_strict{
            entry, table_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.cookie_mask() == 0xffffffffffffffff);
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_entry_and_port_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};
        auto const out_port = std::uint32_t{proto::OFPP_CONTROLLER};
        auto const out_group = 1;

        auto const sut = v13::messages::flow_delete_strict{
            entry, table_id, out_port, out_group
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.cookie_mask() == 0xffffffffffffffff);
        BOOST_TEST(sut.out_port() == out_port);
        BOOST_TEST(sut.out_group() == out_group);
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_table_id_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{proto::OFPTT_ALL};

        auto const sut = v13::messages::flow_delete_strict{
            entry.match(), entry.priority(), table_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_cookie_mask_test)
    {
        auto const priority = std::uint16_t{65535};
        auto const table_id = std::uint8_t{0};
        auto const cookie_mask
            = v13::cookie_mask{0x0f0f0f0f0f0f0f0f, 0xffffffff0000000f};

        auto const sut = v13::messages::flow_delete_strict{
            v13::oxm_match_set{}, priority, table_id, cookie_mask
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == 4);
        BOOST_TEST(sut.priority() == priority);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_out_port_test)
    {
        auto const priority = std::uint16_t{proto::OFP_DEFAULT_PRIORITY};
        auto const table_id = std::uint8_t{1};
        auto const out_port = std::uint32_t{1};
        auto const out_group = 254;

        auto const sut = v13::messages::flow_delete_strict{
            v13::oxm_match_set{}, priority, table_id, out_port, out_group
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == 4);
        BOOST_TEST(sut.priority() == priority);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == out_port);
        BOOST_TEST(sut.out_group() == out_group);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_delete_strict_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_delete_strict_fixture)
    {
        auto src = sut;
        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_delete_strict_fixture)
    {
        auto copy = v13::messages::flow_delete_strict{v13::oxm_match_set{}, 0, 0};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_delete_strict_fixture)
    {
        auto src = sut;
        auto copy = v13::messages::flow_delete_strict{v13::oxm_match_set{}, 0, 0};

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_delete_strict_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_delete_strict, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_delete_strict_fixture)
    {
        auto it = bin_flow_delete_strict.begin();
        auto const it_end = bin_flow_delete_strict.end();

        auto const flow_delete_strict = v13::messages::flow_delete_strict::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_delete_strict.version() == sut.version());
        BOOST_TEST(flow_delete_strict.type() == sut.type());
        BOOST_TEST(flow_delete_strict.length() == sut.length());
        BOOST_TEST(flow_delete_strict.xid() == sut.xid());
        BOOST_TEST(flow_delete_strict.match().length() == sut.match().length());
        BOOST_TEST(flow_delete_strict.priority() == sut.priority());
        BOOST_TEST(flow_delete_strict.table_id() == sut.table_id());
        BOOST_TEST(flow_delete_strict.cookie() == sut.cookie());
        BOOST_TEST(flow_delete_strict.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(flow_delete_strict.out_port() == sut.out_port());
        BOOST_TEST(flow_delete_strict.out_group() == sut.out_group());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_delete_strict_test

BOOST_AUTO_TEST_SUITE_END() // message_test
