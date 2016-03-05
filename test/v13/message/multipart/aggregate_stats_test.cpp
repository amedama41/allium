#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/aggregate_stats.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>

#include "../../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace multipart = v13::messages::multipart;
using proto = v13::protocol;

namespace {

struct aggregate_stats_request_fixture
{
    multipart::aggregate_stats_request sut{
          v13::oxm_match_set{
              v13::oxm_in_port{4}
            , v13::oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
          } // 4 + 8 + 10 = 22
        , proto::OFPTT_ALL
        , v13::cookie_mask{0xf1f2f3f4f5f6f7f8, 0x0f0f0f0f0f0f0f0f}
        , proto::OFPP_TABLE
        , proto::OFPG_MAX
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_aggregate_stats_request
        = "\x04\x12\x00\x48\x12\x34\x56\x78""\x00\x02\x00\x00\x00\x00\x00\x00"
          "\xff\x00\x00\x00\xff\xff\xff\xf9""\xff\xff\xff\x00\x00\x00\x00\x00"
          "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8""\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f"
          "\x00\x01\x00\x16\x80\x00\x00\x04""\x00\x00\x00\x04\x80\x00\x06\x06"
          "\x01\x02\x03\x04\x05\x06\x00\x00"_bin
        ;
};

struct aggregate_stats_reply_fixture
{
    multipart::aggregate_stats_reply sut{
          v13::counters{0xf1f2f3f4f5f6f7f8, 0xffff0000ffff0000}
        , 0x87654321
        , 0x1f3f5f7f
    };
    std::vector<std::uint8_t> bin_aggregate_stats_reply
        = "\x04\x13\x00\x28\x1f\x3f\x5f\x7f""\x00\x02\x00\x00\x00\x00\x00\x00"
          "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8""\xff\xff\x00\x00\xff\xff\x00\x00"
          "\x87\x65\x43\x21\x00\x00\x00\x00"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(aggregate_stats_request_test)

    BOOST_AUTO_TEST_CASE(construct_from_match_test)
    {
        auto const match = v13::oxm_match_set{
              v13::oxm_in_port{4}
            , v13::oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , v13::oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
        }; // 4 + 8 + 10 + 10 = 32
        auto const table_id = std::uint8_t{1};

        auto const sut = multipart::aggregate_stats_request{
            match, table_id
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_aggregate_stats_request)
                                 + 32);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_AGGREGATE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.match().length() == match.length());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_cookie_mask_test)
    {
        auto const match = v13::oxm_match_set{
              v13::oxm_in_port{4}
        }; // 4 + 8 = 12
        auto const table_id = std::uint8_t{proto::OFPTT_ALL};
        auto const cookie_mask = v13::cookie_mask{32, 64};

        auto const sut = multipart::aggregate_stats_request{
            match, table_id, cookie_mask
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_aggregate_stats_request)
                                 + 16);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_AGGREGATE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.match().length() == match.length());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie_mask.value());
        BOOST_TEST(sut.cookie_mask() == cookie_mask.mask());
        BOOST_TEST(sut.out_port() == proto::OFPP_ANY);
        BOOST_TEST(sut.out_group() == proto::OFPG_ANY);
    }

    BOOST_AUTO_TEST_CASE(construct_from_out_port_test)
    {
        auto const match = v13::oxm_match_set{
        }; // 4
        auto const table_id = std::uint8_t{254};
        auto const out_port = std::uint32_t{proto::OFPP_CONTROLLER};
        auto const out_group = std::uint32_t{1};

        auto const sut = multipart::aggregate_stats_request{
            match, table_id, out_port, out_group
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_aggregate_stats_request)
                                 + 8);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_AGGREGATE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.match().length() == match.length());
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == 0);
        BOOST_TEST(sut.cookie_mask() == 0);
        BOOST_TEST(sut.out_port() == out_port);
        BOOST_TEST(sut.out_group() == out_group);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, aggregate_stats_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, aggregate_stats_request_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.match().length() == sut.match().length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.cookie() == sut.cookie());
        BOOST_TEST(copy.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(copy.out_port() == sut.out_port());
        BOOST_TEST(copy.out_group() == sut.out_group());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_aggregate_stats_request)
                                 + 8);
        BOOST_TEST(src.match().length() == 4);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, aggregate_stats_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_aggregate_stats_request, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, aggregate_stats_request_fixture)
    {
        auto it = bin_aggregate_stats_request.begin();
        auto const it_end = bin_aggregate_stats_request.end();

        auto const aggregate_stats_request
            = multipart::aggregate_stats_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(aggregate_stats_request.version() == sut.version());
        BOOST_TEST(aggregate_stats_request.type() == sut.type());
        BOOST_TEST(aggregate_stats_request.length() == sut.length());
        BOOST_TEST(aggregate_stats_request.xid() == sut.xid());
        BOOST_TEST(aggregate_stats_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(aggregate_stats_request.flags() == sut.flags());
        BOOST_TEST(aggregate_stats_request.match().length() == sut.match().length());
        BOOST_TEST(aggregate_stats_request.table_id() == sut.table_id());
        BOOST_TEST(aggregate_stats_request.cookie() == sut.cookie());
        BOOST_TEST(aggregate_stats_request.cookie_mask() == sut.cookie_mask());
        BOOST_TEST(aggregate_stats_request.out_port() == sut.out_port());
        BOOST_TEST(aggregate_stats_request.out_group() == sut.out_group());
    }

BOOST_AUTO_TEST_SUITE_END() // aggregate_stats_request_test


BOOST_AUTO_TEST_SUITE(aggregate_stats_reply_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const counters = v13::counters{32, 48};
        auto const flow_count = std::uint32_t{82};

        auto const sut = multipart::aggregate_stats_reply{
            counters, flow_count
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_aggregate_stats_reply));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_AGGREGATE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.packet_count() == counters.packet_count());
        BOOST_TEST(sut.byte_count() == counters.byte_count());
        BOOST_TEST(sut.flow_count() == flow_count);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, aggregate_stats_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.flow_count() == sut.flow_count());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, aggregate_stats_reply_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.packet_count() == sut.packet_count());
        BOOST_TEST(copy.byte_count() == sut.byte_count());
        BOOST_TEST(copy.flow_count() == sut.flow_count());
        BOOST_TEST(src.length() == sut.length());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, aggregate_stats_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_aggregate_stats_reply, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, aggregate_stats_reply_fixture)
    {
        auto it = bin_aggregate_stats_reply.begin();
        auto const it_end = bin_aggregate_stats_reply.end();

        auto const aggregate_stats_reply
            = multipart::aggregate_stats_reply::decode(it, it_end);

        BOOST_TEST(aggregate_stats_reply.version() == sut.version());
        BOOST_TEST(aggregate_stats_reply.type() == sut.type());
        BOOST_TEST(aggregate_stats_reply.length() == sut.length());
        BOOST_TEST(aggregate_stats_reply.xid() == sut.xid());
        BOOST_TEST(aggregate_stats_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(aggregate_stats_reply.flags() == sut.flags());
        BOOST_TEST(aggregate_stats_reply.packet_count() == sut.packet_count());
        BOOST_TEST(aggregate_stats_reply.byte_count() == sut.byte_count());
        BOOST_TEST(aggregate_stats_reply.flow_count() == sut.flow_count());
    }

BOOST_AUTO_TEST_SUITE_END() // aggregate_stats_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
