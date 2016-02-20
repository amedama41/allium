#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/flow_removed.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

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

struct oxm_match_fixture
{
    v13::oxm_match const match{
          v13::match::oxm_in_port{1}
        , v13::match::oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
        , v13::match::oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
        , v13::match::oxm_eth_type{0x0800, 0x0800}
    };
};

struct flow_removed_fixutre : oxm_match_fixture
{
    v13::messages::flow_removed sut{
          match, 0xff00, 0xf010f010f010f010
        , proto::OFPRR_DELETE
        , proto::OFPTT_MAX
        , v13::elapsed_time{0x10, 0xff0011}
        , v13::timeouts{0xff3e, 0xff13}
        , v13::counters{0xfedcba, 0xfedcba01}
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_flow_removed
        = "\x04\x0b\x00\x58\x12\x34\x56\x78" "\xf0\x10\xf0\x10\xf0\x10\xf0\x10"
          "\xff\x00\x02\xfe\x00\x00\x00\x10" "\x00\xff\x00\x11\xff\x3e\xff\x13"
          "\x00\x00\x00\x00\x00\xfe\xdc\xba" "\x00\x00\x00\x00\xfe\xdc\xba\x01"
          "\x00\x01\x00\x28\x80\x00\x00\x04" "\x00\x00\x00\x01\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x80\x00" "\x08\x06\x11\x12\x13\x14\x15\x16"
          "\x80\x00\x0b\x04\x08\x00\x08\x00"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(flow_removed_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_flow_entry_test, oxm_match_fixture)
    {
        auto const entry = v13::flow_entry{
              match, proto::OFP_DEFAULT_PRIORITY
            , 0xff00ff0012345678
            , v13::instruction_set{}
        };
        auto const reason = proto::OFPRR_HARD_TIMEOUT;
        auto const table_id = 1;
        auto const elapsed_time = v13::elapsed_time{1, 2};
        auto const timeouts = v13::timeouts{10, 30};
        auto const counters = v13::counters{32, 32 * 1500};
        auto const xid = 0xff001200;

        auto const sut = v13::messages::flow_removed{
            entry, reason, table_id, elapsed_time, timeouts, counters, xid
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FLOW_REMOVED);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_flow_removed) + 40);
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.cookie() == entry.cookie());
        BOOST_TEST(sut.priority() == entry.priority());
        BOOST_TEST(sut.reason() == reason);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.duration_sec() == elapsed_time.duration_sec());
        BOOST_TEST(sut.duration_nsec() == elapsed_time.duration_nsec());
        BOOST_TEST(sut.idle_timeout() == timeouts.idle_timeout());
        BOOST_TEST(sut.hard_timeout() == timeouts.hard_timeout());
        BOOST_TEST(sut.packet_count() == counters.packet_count());
        BOOST_TEST(sut.byte_count() == counters.byte_count());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, flow_removed_fixutre)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.reason() == sut.reason());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, flow_removed_fixutre)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.reason() == sut.reason());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_removed)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, flow_removed_fixutre)
    {
        auto copy = v13::messages::flow_removed{
              v13::oxm_match{}, 0, 0, proto::OFPRR_IDLE_TIMEOUT, 0
            , v13::elapsed_time{0, 0}, v13::timeouts{0, 0}, v13::counters{0, 0}
        };

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.reason() == sut.reason());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, flow_removed_fixutre)
    {
        auto src = sut;
        auto copy = v13::messages::flow_removed{
              v13::oxm_match{}, 0, 0, proto::OFPRR_IDLE_TIMEOUT, 0
            , v13::elapsed_time{0, 0}, v13::timeouts{0, 0}, v13::counters{0, 0}
        };

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.priority() == sut.priority());
        BOOST_TEST(copy.reason() == sut.reason());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(copy.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(copy.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_flow_removed)
                                 + sizeof(v13_detail::ofp_match));
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, flow_removed_fixutre)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_flow_removed, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, flow_removed_fixutre)
    {
        auto it = bin_flow_removed.begin();
        auto it_end = bin_flow_removed.end();

        auto flow_removed = v13::messages::flow_removed::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(flow_removed.version() == sut.version());
        BOOST_TEST(flow_removed.type() == sut.type());
        BOOST_TEST(flow_removed.length() == sut.length());
        BOOST_TEST(flow_removed.xid() == sut.xid());
        BOOST_TEST(flow_removed.cookie() == sut.cookie());
        BOOST_TEST(flow_removed.priority() == sut.priority());
        BOOST_TEST(flow_removed.reason() == sut.reason());
        BOOST_TEST(flow_removed.table_id() == sut.table_id());
        BOOST_TEST(flow_removed.duration_sec() == sut.duration_sec());
        BOOST_TEST(flow_removed.duration_nsec() == sut.duration_nsec());
        BOOST_TEST(flow_removed.idle_timeout() == sut.idle_timeout());
        BOOST_TEST(flow_removed.hard_timeout() == sut.hard_timeout());
        BOOST_TEST(flow_removed.packet_count() == sut.packet_count());
        BOOST_TEST(flow_removed.byte_count() == sut.byte_count());
    }

BOOST_AUTO_TEST_SUITE_END() // flow_removed_test

BOOST_AUTO_TEST_SUITE_END() // message_test

