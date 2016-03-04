#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/flow_modify.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

namespace {

auto operator ""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

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

struct flow_modify_fixutre : flow_entry_fixture
{
    v13::messages::flow_modify sut = {
          entry.match()
        , 254
        , entry.instructions()
        , v13::cookie_mask{0xf1f2f3f4f5f6f7f8, 0x0f0f0f0f0f0f0f0f}
        , true
        , proto::OFP_NO_BUFFER
        , 0x12345678
    };

    std::vector<std::uint8_t> bin_flow_modify
        = "\x04\x0e\x00\xb8\x12\x34\x56\x78" "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f" "\xfe\x01\x00\x00\x00\x00\x00\x00"
          "\xff\xff\xff\xff\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x04\x00\x00"

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

struct flow_modify_strict_fixture : flow_entry_fixture
{
    v13::messages::flow_modify_strict sut = {
          entry
        , 254
        , entry.instructions()
        , true
        , proto::OFP_NO_BUFFER
        , 0x12345678
    };

    std::vector<std::uint8_t> bin_flow_modify_strict
        = "\x04\x0e\x00\xb8\x12\x34\x56\x78" "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\xff\xff\xff\xff\xff\xff\xff\xff" "\xfe\x02\x00\x00\x00\x00\x80\x00"
          "\xff\xff\xff\xff\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x04\x00\x00"

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

}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(flow_modify_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_table_id_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};

        auto const sut = v13::messages::flow_modify{
            entry.match(), table_id, entry.instructions()
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(!sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_without_reset_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{0};
        auto const buffer_id = std::uint32_t{0xff1234ff};

        auto const sut = v13::messages::flow_modify{
              entry.match(), table_id, entry.instructions()
            , false, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(!sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_with_reset_count_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{254};
        auto const buffer_id = std::uint32_t{0xff1234ff};

        auto const sut = v13::messages::flow_modify{
              entry.match(), table_id, entry.instructions()
            , true, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_cookie_mask_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{128};
        auto const cookie_mask
            = v13::cookie_mask{0x12ff34ff56ff78ff, 0xf0f0f0f0f0f0f0f0};
        auto const buffer_id = std::uint32_t{0xff1234ff};

        auto const sut = v13::messages::flow_modify{
              entry.match(), table_id, entry.instructions()
            , cookie_mask, false, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(!sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(construct_from_cookie_mask_with_reset_count_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{253};
        auto const cookie_mask
            = v13::cookie_mask{0x12ff34ff56ff78ff, 0xf0f0f0f0f0f0f0f0};
        auto const buffer_id = std::uint32_t{0xff1234ff};

        auto const sut = v13::messages::flow_modify{
              entry.match(), table_id, entry.instructions()
            , cookie_mask, true, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 56 + 8 + 40);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_modify_fixutre)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_modify, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_modify_fixutre)
    {
        auto it = bin_flow_modify.begin();
        auto const it_end = bin_flow_modify.end();

        auto const flow_modify
            = v13::messages::flow_modify::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_modify.version() == sut.version());
        BOOST_TEST(flow_modify.type() == sut.type());
        BOOST_TEST(flow_modify.length() == sut.length());
        BOOST_TEST(flow_modify.xid() == sut.xid());
        BOOST_TEST(flow_modify.match().length() == sut.match().length());
        BOOST_TEST(flow_modify.cookie() == sut.cookie());
        BOOST_TEST(flow_modify.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(flow_modify.table_id() == sut.table_id());
        BOOST_TEST(flow_modify.reset_counter() == sut.reset_counter());
        BOOST_TEST(flow_modify.buffer_id() == sut.buffer_id());
        BOOST_TEST(flow_modify.instructions().length() == sut.instructions().length());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_modify_test


BOOST_AUTO_TEST_SUITE(flow_modify_strict_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};
        auto const instructions
            = v13::instruction_set{v13::instructions::goto_table{3}};
        auto const buffer_id = proto::OFP_NO_BUFFER;

        auto const sut = v13::messages::flow_modify_strict{
            entry, table_id, instructions, true, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 32 + 8);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.cookie_mask() == 0xffffffffffffffff);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == instructions.length());
    }

    BOOST_AUTO_TEST_CASE(construct_from_match_test)
    {
        auto const match = v13::oxm_match_set{
            v13::oxm_eth_type{0x0800}, v13::oxm_ip_proto{17}, v13::oxm_udp_src{52}
        }; // 4 + 6 + 5 + 6 = 21
        auto const priority = std::uint16_t{32};
        auto const table_id = std::uint8_t{1};
        auto const instructions
            = v13::instruction_set{v13::instructions::write_metadata{0xff00}};
        auto const buffer_id = 0xff000;

        auto const sut = v13::messages::flow_modify_strict{
            match, priority, table_id, instructions, false, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 24 + 24);
        BOOST_TEST(sut.match().length() == match.length());
        BOOST_TEST(sut.priority() == priority);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(!sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == instructions.length());
    }

    BOOST_AUTO_TEST_CASE(construct_from_cookie_mask_test)
    {
        auto const match = v13::oxm_match_set{};
        auto const priority = std::uint16_t{0};
        auto const table_id = std::uint8_t{254};
        auto const instructions = v13::instruction_set{};
        auto const buffer_id = 0;
        auto const cookie_mask
            = v13::cookie_mask{0x123456780000, 0xffffffff00000000};

        auto const sut = v13::messages::flow_modify_strict{
              match, priority, table_id, instructions
            , cookie_mask, true, buffer_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_mod) + 8);
        BOOST_TEST(sut.match().length() == match.length());
        BOOST_TEST(sut.priority() == priority);
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.reset_counter());
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.instructions().length() == instructions.length());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_modify_strict_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_modify_strict, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_modify_strict_fixture)
    {
        auto it = bin_flow_modify_strict.begin();
        auto const it_end = bin_flow_modify_strict.end();

        auto const flow_modify_strict
            = v13::messages::flow_modify_strict::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_modify_strict.version() == sut.version());
        BOOST_TEST(flow_modify_strict.type() == sut.type());
        BOOST_TEST(flow_modify_strict.length() == sut.length());
        BOOST_TEST(flow_modify_strict.match().length() == sut.match().length());
        BOOST_TEST(flow_modify_strict.priority() == sut.priority());
        BOOST_TEST(flow_modify_strict.cookie() == sut.cookie());
        BOOST_TEST(flow_modify_strict.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(flow_modify_strict.table_id() == sut.table_id());
        BOOST_TEST(flow_modify_strict.reset_counter() == sut.reset_counter());
        BOOST_TEST(flow_modify_strict.buffer_id() == sut.buffer_id());
        BOOST_TEST(flow_modify_strict.instructions().length() == sut.instructions().length());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_modify_strict_test

BOOST_AUTO_TEST_SUITE_END() // message_test
