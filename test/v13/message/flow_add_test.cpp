#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/flow_add.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

namespace {

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
        , v13::instruction_set{
              v13::instructions::apply_actions{
                  v13::actions::set_field{v13::oxm_eth_dst{eth_src}}
                , v13::actions::set_field{v13::oxm_eth_src{eth_dst}}
                , v13::actions::output{4}
              } // 8 + 16 + 16 + 16 = 56
            , v13::instructions::clear_actions{} // 8
            , v13::instructions::write_actions{
                  v13::actions::set_field{v13::oxm_eth_dst{eth_src}}
                , v13::actions::set_field{v13::oxm_eth_src{eth_dst}}
              } // 8 + 16 + 16 = 40
          } // 56 + 8 + 40 = 104
    };
};

struct flow_add_fixture : flow_entry_fixture {
    v13::messages::flow_add sut = {
          entry
        , 0
        , v13::timeouts{0x1234, 0x5678}
        , proto::OFPFF_SEND_FLOW_REM | proto::OFPFF_NO_BYT_COUNTS
        , 0xff12ff34
        , 0xff56ff78
    };

    std::vector<std::uint8_t> bin_flow_add
        = "\x04\x0e\x00\xb8\xff\x56\xff\x78" "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x12\x34\x56\x78\x80\x00"
          "\xff\x12\xff\x34\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x11\x00\x00"

          "\x00\x01\x00\x20\x80\x00\x00\x04" "\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00" "\x08\x06\x11\x12\x13\x14\x15\x16"

          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06" "\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06" "\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04" "\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06" "\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06" "\x01\x02\x03\x04\x05\x06\x00\x00"_bin
        ;
};

static auto const base_flow_mod_size
    = sizeof(v13_detail::ofp_flow_mod) + sizeof(v13_detail::ofp_match);

}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(flow_add_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_lvalue_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};
        auto const flags = std::uint16_t{
            proto::OFPFF_SEND_FLOW_REM | proto::OFPFF_CHECK_OVERLAP
        };
        auto const timeouts = v13::timeouts{32, 24};
        auto const buffer_id = 3;

        auto const sut = v13::messages::flow_add{
            entry, table_id, timeouts, flags, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == 48 + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.idle_timeout() == timeouts.idle_timeout());
        BOOST_TEST(sut.hard_timeout() == timeouts.hard_timeout());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_rvalue_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};
        auto const flags = std::uint16_t{
            proto::OFPFF_SEND_FLOW_REM | proto::OFPFF_CHECK_OVERLAP
        };
        auto const timeouts = v13::timeouts{32, 24};
        auto const buffer_id = 3;
        auto src = entry;

        auto const sut = v13::messages::flow_add{
            std::move(src), table_id, timeouts, flags, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == 48 + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.idle_timeout() == timeouts.idle_timeout());
        BOOST_TEST(sut.hard_timeout() == timeouts.hard_timeout());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());

        BOOST_TEST(src.match().length() == 4);
        BOOST_TEST(src.instructions().length() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_table_id_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{127};

        auto const sut = v13::messages::flow_add{entry, table_id};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == 48 + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.idle_timeout() == 0);
        BOOST_TEST(sut.hard_timeout() == 0);
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_timeouts_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{254};
        auto const timeouts = v13::timeouts{128, 64};

        auto const sut = v13::messages::flow_add{entry, table_id, timeouts};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == 48 + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.idle_timeout() == timeouts.idle_timeout());
        BOOST_TEST(sut.hard_timeout() == timeouts.hard_timeout());
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_flags_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{0};
        auto const flags = std::uint16_t{proto::OFPFF_CHECK_OVERLAP};

        auto const sut = v13::messages::flow_add{entry, table_id, flags};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == 48 + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.idle_timeout() == 0);
        BOOST_TEST(sut.hard_timeout() == 0);
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_add_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_add_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());

        BOOST_TEST(src.length() == base_flow_mod_size);
        BOOST_TEST(src.match().length() == 4);
        BOOST_TEST(src.instructions().length() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_add_fixture)
    {
        auto const empty_entry
            = v13::flow_entry{v13::oxm_match_set{}, 0, 0, v13::instruction_set{}};
        auto copy = v13::messages::flow_add{empty_entry, 0, 0};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_add_fixture)
    {
        auto const empty_entry
            = v13::flow_entry{v13::oxm_match_set{}, 0, 0, v13::instruction_set{}};
        auto copy = v13::messages::flow_add{empty_entry, 0, 0};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());

        BOOST_TEST(src.length() == base_flow_mod_size);
        BOOST_TEST(src.match().length() == 4);
        BOOST_TEST(src.instructions().length() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_add_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_add, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_add_fixture)
    {
        auto it = bin_flow_add.begin();
        auto const it_end = bin_flow_add.end();

        auto const flow_add = v13::messages::flow_add::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_add.version() == sut.version());
        BOOST_TEST(flow_add.type() == sut.type());
        BOOST_TEST(flow_add.length() == sut.length());
        BOOST_TEST(flow_add.xid() == sut.xid());
        BOOST_TEST(flow_add.match().length() == sut.match().length());
        BOOST_TEST(flow_add.priority() == sut.priority());
        BOOST_TEST(flow_add.cookie() == sut.cookie());
        BOOST_TEST(flow_add.table_id() == sut.table_id());
        BOOST_TEST(flow_add.flags() == sut.flags());
        BOOST_TEST(flow_add.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(flow_add.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(flow_add.buffer_id() == sut.buffer_id());
        BOOST_TEST(flow_add.instructions().length() == sut.instructions().length());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_add_test

BOOST_AUTO_TEST_SUITE_END() // message_test

