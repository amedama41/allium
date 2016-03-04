#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/port_stats.hpp>
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

struct port_stats_fixutre
{
    multipart::port_stats sut{
          proto::OFPP_MAX
        , 0x12 // rx_packets
        , 0x34 // tx_packets
        , 0x56 // rx_bytes
        , 0x78 // tx_bytes
        , 0x9a // rx_dropped
        , 0xbc // tx_dropped
        , 0xde // rx_errors
        , 0xf0 // tx_errors
        , 0xf1f2f3f4f5f6f7f8 // rx_frame_err
        , 0x0f0f0f0f0f0f0f0f // rx_over_err
        , 0x1f2f3f4f5f6f7f8f // rx_crc_err
        , 0x1234567887654321 // collisions
        , v13::elapsed_time{0x01020304, 0x05060708}
    };
    std::vector<std::uint8_t> bin_port_stats
        = "\xff\xff\xff\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x12"
          "\x00\x00\x00\x00\x00\x00\x00\x34""\x00\x00\x00\x00\x00\x00\x00\x56"
          "\x00\x00\x00\x00\x00\x00\x00\x78""\x00\x00\x00\x00\x00\x00\x00\x9a"
          "\x00\x00\x00\x00\x00\x00\x00\xbc""\x00\x00\x00\x00\x00\x00\x00\xde"
          "\x00\x00\x00\x00\x00\x00\x00\xf0""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f""\x1f\x2f\x3f\x4f\x5f\x6f\x7f\x8f"
          "\x12\x34\x56\x78\x87\x65\x43\x21""\x01\x02\x03\x04\x05\x06\x07\x08"
          ""_bin
        ;
};

struct port_stats_request_fixture
{
    multipart::port_stats_request sut{1, 0x12345678};
    std::vector<std::uint8_t> bin_port_stats_request
        = "\x04\x12\x00\x18\x12\x34\x56\x78""\x00\x04\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x01\x00\x00\x00\x00"
          ""_bin
        ;
};

struct port_stats_reply_fixture : port_stats_fixutre
{
    multipart::port_stats_reply sut{
          std::vector<multipart::port_stats>(3, port_stats_fixutre::sut)
        , 0
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_port_stats_reply
        = "\x04\x13\x01\x60\x12\x34\x56\x78""\x00\x04\x00\x00\x00\x00\x00\x00"

          "\xff\xff\xff\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x12"
          "\x00\x00\x00\x00\x00\x00\x00\x34""\x00\x00\x00\x00\x00\x00\x00\x56"
          "\x00\x00\x00\x00\x00\x00\x00\x78""\x00\x00\x00\x00\x00\x00\x00\x9a"
          "\x00\x00\x00\x00\x00\x00\x00\xbc""\x00\x00\x00\x00\x00\x00\x00\xde"
          "\x00\x00\x00\x00\x00\x00\x00\xf0""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f""\x1f\x2f\x3f\x4f\x5f\x6f\x7f\x8f"
          "\x12\x34\x56\x78\x87\x65\x43\x21""\x01\x02\x03\x04\x05\x06\x07\x08"

          "\xff\xff\xff\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x12"
          "\x00\x00\x00\x00\x00\x00\x00\x34""\x00\x00\x00\x00\x00\x00\x00\x56"
          "\x00\x00\x00\x00\x00\x00\x00\x78""\x00\x00\x00\x00\x00\x00\x00\x9a"
          "\x00\x00\x00\x00\x00\x00\x00\xbc""\x00\x00\x00\x00\x00\x00\x00\xde"
          "\x00\x00\x00\x00\x00\x00\x00\xf0""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f""\x1f\x2f\x3f\x4f\x5f\x6f\x7f\x8f"
          "\x12\x34\x56\x78\x87\x65\x43\x21""\x01\x02\x03\x04\x05\x06\x07\x08"

          "\xff\xff\xff\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x12"
          "\x00\x00\x00\x00\x00\x00\x00\x34""\x00\x00\x00\x00\x00\x00\x00\x56"
          "\x00\x00\x00\x00\x00\x00\x00\x78""\x00\x00\x00\x00\x00\x00\x00\x9a"
          "\x00\x00\x00\x00\x00\x00\x00\xbc""\x00\x00\x00\x00\x00\x00\x00\xde"
          "\x00\x00\x00\x00\x00\x00\x00\xf0""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f""\x1f\x2f\x3f\x4f\x5f\x6f\x7f\x8f"
          "\x12\x34\x56\x78\x87\x65\x43\x21""\x01\x02\x03\x04\x05\x06\x07\x08"

          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(port_stats_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port_no = std::uint32_t{1};
        auto const rx_packets = std::uint64_t{12};
        auto const tx_packets = std::uint64_t{34};
        auto const rx_bytes = std::uint64_t{12 * 1500};
        auto const tx_bytes = std::uint64_t{34 * 1500};
        auto const rx_dropped = std::uint64_t{3};
        auto const tx_dropped = std::uint64_t{2};
        auto const rx_errors = std::uint64_t{3434};
        auto const tx_errors = std::uint64_t{4343};
        auto const rx_frame_err = std::uint64_t{87};
        auto const rx_over_err = std::uint64_t{67};
        auto const rx_crc_err = std::uint64_t{90};
        auto const collisions = std::uint64_t{123456};
        auto const elapsed_time = v13::elapsed_time{123, 456};

        auto const sut = multipart::port_stats{
              port_no
            , rx_packets, tx_packets
            , rx_bytes, tx_bytes
            , rx_dropped, tx_dropped
            , rx_errors, tx_errors
            , rx_frame_err
            , rx_over_err
            , rx_crc_err
            , collisions
            , elapsed_time
        };

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_port_stats));
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.rx_packets() == rx_packets);
        BOOST_TEST(sut.tx_packets() == tx_packets);
        BOOST_TEST(sut.rx_bytes() == rx_bytes);
        BOOST_TEST(sut.tx_bytes() == tx_bytes);
        BOOST_TEST(sut.rx_dropped() == rx_dropped);
        BOOST_TEST(sut.tx_dropped() == tx_dropped);
        BOOST_TEST(sut.rx_errors() == rx_errors);
        BOOST_TEST(sut.tx_errors() == tx_errors);
        BOOST_TEST(sut.rx_frame_errors() == rx_frame_err);
        BOOST_TEST(sut.rx_over_errors() == rx_over_err);
        BOOST_TEST(sut.rx_crc_errors() == rx_crc_err);
        BOOST_TEST(sut.collisions() == collisions);
        BOOST_TEST(sut.duration_sec() == elapsed_time.duration_sec());
        BOOST_TEST(sut.duration_nsec() == elapsed_time.duration_nsec());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_stats_fixutre)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.rx_packets() == sut.rx_packets());
        BOOST_TEST(copy.tx_packets() == sut.tx_packets());
        BOOST_TEST(copy.rx_bytes() == sut.rx_bytes());
        BOOST_TEST(copy.tx_bytes() == sut.tx_bytes());
        BOOST_TEST(copy.rx_dropped() == sut.rx_dropped());
        BOOST_TEST(copy.tx_dropped() == sut.tx_dropped());
        BOOST_TEST(copy.rx_errors() == sut.rx_errors());
        BOOST_TEST(copy.tx_errors() == sut.tx_errors());
        BOOST_TEST(copy.rx_frame_errors() == sut.rx_frame_errors());
        BOOST_TEST(copy.rx_over_errors() == sut.rx_over_errors());
        BOOST_TEST(copy.rx_crc_errors() == sut.rx_crc_errors());
        BOOST_TEST(copy.collisions() == sut.collisions());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, port_stats_fixutre)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.rx_packets() == sut.rx_packets());
        BOOST_TEST(copy.tx_packets() == sut.tx_packets());
        BOOST_TEST(copy.rx_bytes() == sut.rx_bytes());
        BOOST_TEST(copy.tx_bytes() == sut.tx_bytes());
        BOOST_TEST(copy.rx_dropped() == sut.rx_dropped());
        BOOST_TEST(copy.tx_dropped() == sut.tx_dropped());
        BOOST_TEST(copy.rx_errors() == sut.rx_errors());
        BOOST_TEST(copy.tx_errors() == sut.tx_errors());
        BOOST_TEST(copy.rx_frame_errors() == sut.rx_frame_errors());
        BOOST_TEST(copy.rx_over_errors() == sut.rx_over_errors());
        BOOST_TEST(copy.rx_crc_errors() == sut.rx_crc_errors());
        BOOST_TEST(copy.collisions() == sut.collisions());
        BOOST_TEST(copy.duration_sec() == sut.duration_sec());
        BOOST_TEST(copy.duration_nsec() == sut.duration_nsec());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_stats_fixutre)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_port_stats, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_stats_fixutre)
    {
        auto it = bin_port_stats.begin();
        auto const it_end = bin_port_stats.end();

        auto const port_stats = multipart::port_stats::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(port_stats.length() == sut.length());
        BOOST_TEST(port_stats.port_no() == sut.port_no());
        BOOST_TEST(port_stats.rx_packets() == sut.rx_packets());
        BOOST_TEST(port_stats.tx_packets() == sut.tx_packets());
        BOOST_TEST(port_stats.rx_bytes() == sut.rx_bytes());
        BOOST_TEST(port_stats.tx_bytes() == sut.tx_bytes());
        BOOST_TEST(port_stats.rx_dropped() == sut.rx_dropped());
        BOOST_TEST(port_stats.tx_dropped() == sut.tx_dropped());
        BOOST_TEST(port_stats.rx_errors() == sut.rx_errors());
        BOOST_TEST(port_stats.tx_errors() == sut.tx_errors());
        BOOST_TEST(port_stats.rx_frame_errors() == sut.rx_frame_errors());
        BOOST_TEST(port_stats.rx_over_errors() == sut.rx_over_errors());
        BOOST_TEST(port_stats.rx_crc_errors() == sut.rx_crc_errors());
        BOOST_TEST(port_stats.collisions() == sut.collisions());
        BOOST_TEST(port_stats.duration_sec() == sut.duration_sec());
        BOOST_TEST(port_stats.duration_nsec() == sut.duration_nsec());
    }

BOOST_AUTO_TEST_SUITE_END() // port_stats_test


BOOST_AUTO_TEST_SUITE(port_stats_request_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port_no = std::uint32_t{proto::OFPP_ANY};

        auto const sut = multipart::port_stats_request{port_no};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request)
                                 + sizeof(v13_detail::ofp_port_stats_request));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_PORT_STATS);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.port_no() == port_no);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_stats_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.port_no() == sut.port_no());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, port_stats_request_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(src.length() == sut.length());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_stats_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_port_stats_request, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_stats_request_fixture)
    {
        auto it = bin_port_stats_request.begin();
        auto const it_end = bin_port_stats_request.end();

        auto const port_stats_request
            = multipart::port_stats_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(port_stats_request.version() == sut.version());
        BOOST_TEST(port_stats_request.type() == sut.type());
        BOOST_TEST(port_stats_request.length() == sut.length());
        BOOST_TEST(port_stats_request.xid() == sut.xid());
        BOOST_TEST(port_stats_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(port_stats_request.flags() == sut.flags());
        BOOST_TEST(port_stats_request.port_no() == sut.port_no());
    }

BOOST_AUTO_TEST_SUITE_END() // port_stats_request_test


BOOST_AUTO_TEST_SUITE(port_stats_reply_test)

    BOOST_FIXTURE_TEST_CASE(construct_test, port_stats_fixutre)
    {
        auto const size = std::size_t{2};
        auto const port_stats = std::vector<multipart::port_stats>(
                size, port_stats_fixutre::sut);

        auto const sut = multipart::port_stats_reply{port_stats};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_port_stats) * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_PORT_STATS);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(construct_with_flag_test, port_stats_fixutre)
    {
        auto const size = std::size_t{0};
        auto const port_stats = std::vector<multipart::port_stats>(
                size, port_stats_fixutre::sut);
        auto const flags = std::uint16_t{proto::OFPMPF_REPLY_MORE};

        auto const sut = multipart::port_stats_reply{port_stats, flags};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_port_stats) * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_PORT_STATS);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_stats_reply_fixture)
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

    BOOST_FIXTURE_TEST_CASE(move_construct_test, port_stats_reply_fixture)
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

    BOOST_FIXTURE_TEST_CASE(encode_test, port_stats_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_port_stats_reply, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_stats_reply_fixture)
    {
        auto it = bin_port_stats_reply.begin();
        auto const it_end = bin_port_stats_reply.end();

        auto const port_stats_reply
            = multipart::port_stats_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(port_stats_reply.version() == sut.version());
        BOOST_TEST(port_stats_reply.type() == sut.type());
        BOOST_TEST(port_stats_reply.length() == sut.length());
        BOOST_TEST(port_stats_reply.xid() == sut.xid());
        BOOST_TEST(port_stats_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(port_stats_reply.flags() == sut.flags());
        BOOST_TEST(port_stats_reply.size() == sut.size());
    }

BOOST_AUTO_TEST_SUITE_END() // port_stats_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
