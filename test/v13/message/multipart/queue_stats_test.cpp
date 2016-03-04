#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/queue_stats.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

#include "../../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace multipart = v13::messages::multipart;
using proto = v13::protocol;

namespace {

struct queue_stats_fixture
{
    multipart::queue_stats sut{
          0x12345678, proto::OFPP_MAX
        , 0x0102030405060708
        , 0xf1f2f3f4f5f6f7f8
        , 0xff00ff00ff00ff00
        , v13::elapsed_time{0x1234, 0x5678}
    };
    std::vector<std::uint8_t> binary
        = "\xff\xff\xff\x00\x12\x34\x56\x78""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08""\xff\x00\xff\x00\xff\x00\xff\x00"
          "\x00\x00\x12\x34\x00\x00\x56\x78"
          ""_bin
        ;
};

struct queue_stats_request_fixture
{
    multipart::queue_stats_request sut{
        0x12345678, proto::OFPP_MAX, 0x12345678
    };
    std::vector<std::uint8_t> binary
        = "\x04\x12\x00\x18\x12\x34\x56\x78""\x00\x05\x00\x00\x00\x00\x00\x00"
          "\xff\xff\xff\x00\x12\x34\x56\x78"
          ""_bin
        ;
};

struct queue_stats_reply_fixture
{
    multipart::queue_stats_reply sut{
          {
              multipart::queue_stats{
                  0x1, 0x1
                , 0x0102030405060708, 0xf1f2f3f4f5f6f7f8, 0xff00ff00ff00ff00
                , v13::elapsed_time{0x10001234, 0x10005678}
              }
            , multipart::queue_stats{
                  0xfffffffe, 0x1
                , 0x1112131415161718, 0xe1e2e3e4e5e6e7e8, 0xee00ee00ee00ee00
                , v13::elapsed_time{0x20001234, 0x20005678}
              }
            , multipart::queue_stats{
                  0x2, proto::OFPP_MAX
                , 0x8182838485868788, 0xa1a2a3a4a5a6a7a8, 0xbf00bf00bf00bf00
                , v13::elapsed_time{0x30001234, 0x30005678}
              }
          }
        , proto::OFPMPF_REPLY_MORE
        , 0x12345678
    };
    std::vector<std::uint8_t> binary
        = "\x04\x13\x00\x88\x12\x34\x56\x78""\x00\x05\x00\x01\x00\x00\x00\x00"

          "\x00\x00\x00\x01\x00\x00\x00\x01""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08""\xff\x00\xff\x00\xff\x00\xff\x00"
          "\x10\x00\x12\x34\x10\x00\x56\x78"

          "\x00\x00\x00\x01\xff\xff\xff\xfe""\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8"
          "\x11\x12\x13\x14\x15\x16\x17\x18""\xee\x00\xee\x00\xee\x00\xee\x00"
          "\x20\x00\x12\x34\x20\x00\x56\x78"

          "\xff\xff\xff\x00\x00\x00\x00\x02""\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8"
          "\x81\x82\x83\x84\x85\x86\x87\x88""\xbf\x00\xbf\x00\xbf\x00\xbf\x00"
          "\x30\x00\x12\x34\x30\x00\x56\x78"
          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(queue_stats_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const queue_id = std::uint32_t{3};
        auto const port_no = std::uint32_t{1};
        auto const tx_packets = std::uint64_t{1234};
        auto const tx_bytes = std::uint64_t{5678};
        auto const tx_errors = std::uint64_t{987654};
        auto const elapsed_time = v13::elapsed_time{32, 45};

        auto const sut = multipart::queue_stats{
            queue_id, port_no, tx_packets, tx_bytes, tx_errors, elapsed_time
        };

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_queue_stats));
        BOOST_TEST(sut.queue_id() == queue_id);
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.tx_packets() == tx_packets);
        BOOST_TEST(sut.tx_bytes() == tx_bytes);
        BOOST_TEST(sut.tx_errors() == tx_errors);
        BOOST_TEST(sut.duration_sec() == elapsed_time.duration_sec());
        BOOST_TEST(sut.duration_nsec() == elapsed_time.duration_nsec());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, queue_stats_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.tx_packets() == sut.tx_packets());
        BOOST_TEST(copy.tx_bytes() == sut.tx_bytes());
        BOOST_TEST(copy.tx_errors() == sut.tx_errors());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, queue_stats_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, queue_stats_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const queue_stats = multipart::queue_stats::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(queue_stats.length() == sut.length());
        BOOST_TEST(queue_stats.queue_id() == sut.queue_id());
        BOOST_TEST(queue_stats.port_no() == sut.port_no());
        BOOST_TEST(queue_stats.tx_packets() == sut.tx_packets());
        BOOST_TEST(queue_stats.tx_bytes() == sut.tx_bytes());
        BOOST_TEST(queue_stats.tx_errors() == sut.tx_errors());
        BOOST_TEST(queue_stats.duration_sec() == sut.duration_sec());
        BOOST_TEST(queue_stats.duration_nsec() == sut.duration_nsec());
    }

BOOST_AUTO_TEST_SUITE_END() // queue_stats_test


BOOST_AUTO_TEST_SUITE(queue_stats_request_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const queue_id = std::uint32_t{93283};
        auto const port_no = std::uint32_t{7833};

        auto const sut = multipart::queue_stats_request{queue_id, port_no};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_queue_stats_request));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_QUEUE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.queue_id() == queue_id);
        BOOST_TEST(sut.port_no() == port_no);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, queue_stats_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, queue_stats_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, queue_stats_request_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const queue_stats_request
            = multipart::queue_stats_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(queue_stats_request.version() == sut.version());
        BOOST_TEST(queue_stats_request.type() == sut.type());
        BOOST_TEST(queue_stats_request.length() == sut.length());
        BOOST_TEST(queue_stats_request.xid() == sut.xid());
        BOOST_TEST(queue_stats_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(queue_stats_request.flags() == sut.flags());
        BOOST_TEST(queue_stats_request.queue_id() == sut.queue_id());
        BOOST_TEST(queue_stats_request.port_no() == sut.port_no());
    }

BOOST_AUTO_TEST_SUITE_END() // queue_stats_request_test


BOOST_AUTO_TEST_SUITE(queue_stats_reply_test)

    BOOST_FIXTURE_TEST_CASE(construct_test, queue_stats_fixture)
    {
        auto const size = 2;
        auto const queue_stats = std::vector<multipart::queue_stats>(
                size, queue_stats_fixture::sut);

        auto const sut = multipart::queue_stats_reply{queue_stats};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + queue_stats_fixture::sut.length() * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_QUEUE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, queue_stats_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, queue_stats_reply_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_reply));
        BOOST_TEST(src.size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, queue_stats_reply_fixture)
    {
        auto copy = multipart::queue_stats_reply{{}};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, queue_stats_reply_fixture)
    {
        auto copy = multipart::queue_stats_reply{{}};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.size() == sut.size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_multipart_reply));
        BOOST_TEST(src.size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, queue_stats_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, queue_stats_reply_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const queue_stats_reply
            = multipart::queue_stats_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(queue_stats_reply.version() == sut.version());
        BOOST_TEST(queue_stats_reply.type() == sut.type());
        BOOST_TEST(queue_stats_reply.length() == sut.length());
        BOOST_TEST(queue_stats_reply.xid() == sut.xid());
        BOOST_TEST(queue_stats_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(queue_stats_reply.flags() == sut.flags());
        BOOST_TEST(queue_stats_reply.size() == sut.size());
    }

BOOST_AUTO_TEST_SUITE_END() // queue_stats_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
