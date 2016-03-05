#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/flow_stats.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>

#include "../../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace multipart = v13::messages::multipart;
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

struct flow_stats_fixture : flow_entry_fixture
{
    multipart::flow_stats sut = {
          entry, 0x01
        , proto::OFPFF_SEND_FLOW_REM | proto::OFPFF_NO_PKT_COUNTS
        , v13::timeouts{0x1234, 0}
        , v13::elapsed_time{0x12345678, 0x87654321}
        , v13::counters{0, 0x1234567887654321}
    };
    std::vector<std::uint8_t> bin_flow_stats
        = "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"

          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"

          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"_bin
        ;
};

struct flow_stats_request_fixture : flow_entry_fixture
{
    multipart::flow_stats_request sut = {
        entry, 0xf3, proto::OFPP_FLOOD, proto::OFPG_MAX, 0x12345678
    };
    std::vector<std::uint8_t> bin_flow_stats_request
        = "\x04\x12\x00\x50\x12\x34\x56\x78""\x00\x01\x00\x00\x00\x00\x00\x00"
          "\xf3\x00\x00\x00\xff\xff\xff\xfb""\xff\xff\xff\x00\x00\x00\x00\x00"
          "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8""\xff\xff\xff\xff\xff\xff\xff\xff"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"_bin
        ;
};

struct flow_stats_reply_fixture : flow_stats_fixture
{
    multipart::flow_stats_reply sut = multipart::flow_stats_reply{
          std::vector<multipart::flow_stats>(6, flow_stats_fixture::sut)
        , proto::OFPMPF_REPLY_MORE
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_flow_stats_reply
        = "\x04\x13\x04\x60\x12\x34\x56\x78""\x00\x01\x00\x01\x00\x00\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"

          "\x00\xb8\x01\x00\x12\x34\x56\x78""\x87\x65\x43\x21\x80\x00\x12\x34"
          "\x00\x00\x00\x09\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x12\x34\x56\x78\x87\x65\x43\x21"
          "\x00\x01\x00\x20\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00""\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x00\x04\x00\x38\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"
          "\x00\x00\x00\x10\x00\x00\x00\x04""\xff\xff\x00\x00\x00\x00\x00\x00"
          "\x00\x05\x00\x08\x00\x00\x00\x00"
          "\x00\x03\x00\x28\x00\x00\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x06\x06""\x11\x12\x13\x14\x15\x16\x00\x00"
          "\x00\x19\x00\x10\x80\x00\x08\x06""\x01\x02\x03\x04\x05\x06\x00\x00"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(flow_stats_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{proto::OFPTT_MAX};
        auto const flags = std::uint16_t(
            proto::OFPFF_SEND_FLOW_REM | proto::OFPFF_CHECK_OVERLAP);
        auto const timeouts = v13::timeouts{0x1234, 0x5678};
        auto const elapsed_time = v13::elapsed_time{3624, 5432};
        auto const counters
            = v13::counters{0xf1f2f3f4f5f6f7f8, 0x1234567887654321};

        auto const sut = multipart::flow_stats{
            entry, table_id, flags, timeouts, elapsed_time, counters
        };

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_stats) + 32 + 104);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.idle_timeout() == timeouts.idle_timeout());
        BOOST_TEST(sut.hard_timeout() == timeouts.hard_timeout());
        BOOST_TEST(sut.duration_sec() == elapsed_time.duration_sec());
        BOOST_TEST(sut.duration_nsec() == elapsed_time.duration_nsec());
        BOOST_TEST(sut.packet_count() == counters.packet_count());
        BOOST_TEST(sut.byte_count() == counters.byte_count());
        BOOST_TEST(sut.instructions().length() == entry.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_stats_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_stats_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_stats)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
        BOOST_TEST(src.instructions().length() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_stats_fixture)
    {
        auto copy = multipart::flow_stats{
              v13::flow_entry{v13::oxm_match_set{}, 0, 0, v13::instruction_set{}}
            , 0, 0
            , v13::timeouts{0, 0}, v13::elapsed_time{0, 0}, v13::counters{0, 0}
        };

        copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_stats_fixture)
    {
        auto copy = multipart::flow_stats{
              v13::flow_entry{v13::oxm_match_set{}, 0, 0, v13::instruction_set{}}
            , 0, 0
            , v13::timeouts{0, 0}, v13::elapsed_time{0, 0}, v13::counters{0, 0}
        };
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.instructions().length() == sut.instructions().length());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_stats)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
        BOOST_TEST(src.instructions().length() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_stats_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_stats, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_stats_fixture)
    {
        auto it = bin_flow_stats.begin();
        auto const it_end = bin_flow_stats.end();

        auto const flow_stats = multipart::flow_stats::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_stats.length() == sut.length());
        BOOST_TEST(flow_stats.match().length() == sut.match().length());
        BOOST_TEST(flow_stats.priority() == sut.priority());
        BOOST_TEST(flow_stats.cookie() == sut.cookie());
        BOOST_TEST(flow_stats.table_id() == sut.table_id());
        BOOST_TEST(flow_stats.flags() == sut.flags());
        BOOST_TEST(flow_stats.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(flow_stats.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(flow_stats.duration_sec() == sut.duration_sec());
        BOOST_TEST(flow_stats.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(flow_stats.packet_count() == sut.packet_count());
        BOOST_TEST(flow_stats.byte_count() == sut.byte_count());
        BOOST_TEST(flow_stats.instructions().length() == sut.instructions().length());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_stats_test

BOOST_AUTO_TEST_SUITE(flow_stats_request_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_entry_test, flow_entry_fixture)
    {
        auto const table_id = std::uint8_t{1};

        auto const sut = multipart::flow_stats_request{
            entry, table_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_flow_stats_request)
                                 + entry.match().length());
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_FLOW);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.match().length() == entry.match().length());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.cookie_mask() == 0xffffffffffffffff);
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_stats_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_stats_request_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_flow_stats_request)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_stats_request_fixture)
    {
        auto copy = multipart::flow_stats_request{v13::oxm_match_set{}, 0};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_stats_request_fixture)
    {
        auto copy = multipart::flow_stats_request{v13::oxm_match_set{}, 0};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_flow_stats_request)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_stats_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_stats_request, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_stats_request_fixture)
    {
        auto it = bin_flow_stats_request.begin();
        auto const it_end = bin_flow_stats_request.end();

        auto const flow_stats_request
            = multipart::flow_stats_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_stats_request.version() == sut.version());
        BOOST_TEST(flow_stats_request.type() == sut.type());
        BOOST_TEST(flow_stats_request.length() == sut.length());
        BOOST_TEST(flow_stats_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(flow_stats_request.flags() == sut.flags());
        BOOST_TEST(flow_stats_request.match().length() == sut.match().length());
        BOOST_TEST(flow_stats_request.table_id() == sut.table_id());
        BOOST_TEST(flow_stats_request.cookie() == sut.cookie());
        BOOST_TEST(flow_stats_request.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(flow_stats_request.out_port() == sut.out_port());
        BOOST_TEST(flow_stats_request.out_group() == sut.out_group());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_stats_request_test


BOOST_AUTO_TEST_SUITE(flow_stats_reply_test)

    BOOST_FIXTURE_TEST_CASE(constructor_test, flow_stats_fixture)
    {
        auto const size = std::size_t{32};
        auto stats = std::vector<multipart::flow_stats>(size, sut);
        auto const flags = std::uint16_t{proto::OFPMPF_REPLY_MORE};

        auto const sut = multipart::flow_stats_reply{stats, flags};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + flow_stats_fixture::sut.length() * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_FLOW);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_stats_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_stats_reply_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_reply));
        BOOST_TEST(src.size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_stats_reply_fixture)
    {
        auto copy = multipart::flow_stats_reply{
            std::vector<multipart::flow_stats>{}
        };

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_stats_reply_fixture)
    {
        auto copy = multipart::flow_stats_reply{
            std::vector<multipart::flow_stats>{}
        };
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_reply));
        BOOST_TEST(src.size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_stats_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_stats_reply, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_stats_reply_fixture)
    {
        auto it = bin_flow_stats_reply.begin();
        auto const it_end = bin_flow_stats_reply.end();

        auto const flow_stats_reply
            = multipart::flow_stats_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_stats_reply.version() == sut.version());
        BOOST_TEST(flow_stats_reply.type() == sut.type());
        BOOST_TEST(flow_stats_reply.length() == sut.length());
        BOOST_TEST(flow_stats_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(flow_stats_reply.flags() == sut.flags());
        BOOST_TEST(flow_stats_reply.size() == sut.size());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_stats_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
