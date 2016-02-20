#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/table_stats.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace multipart = v13::messages::multipart;
using proto = v13::protocol;

namespace {

auto operator ""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

struct table_stats_fixture
{
    multipart::table_stats sut{
        proto::OFPTT_MAX, 0x12345678, 0xf1f2f3f4f5f6f7f8, 0x0102030405060708
    };
    std::vector<std::uint8_t> bin_table_stats
        = "\xfe\x00\x00\x00\x12\x34\x56\x78""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08"_bin
        ;
};

struct table_stats_request_fixture
{
    multipart::table_stats_request sut{0x12345678};
    std::vector<std::uint8_t> bin_table_stats_request
        = "\x04\x12\x00\x10\x12\x34\x56\x78""\x00\x03\x00\x00\x00\x00\x00\x00"_bin
        ;
};

struct table_stats_reply_fixture : table_stats_fixture
{
    multipart::table_stats_reply sut{
          std::vector<multipart::table_stats>(3, table_stats_fixture::sut)
        , proto::OFPMPF_REPLY_MORE
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_table_stats_reply
        = "\x04\x13\x00\x58\x12\x34\x56\x78""\x00\x03\x00\x01\x00\x00\x00\x00"
          "\xfe\x00\x00\x00\x12\x34\x56\x78""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08"
          "\xfe\x00\x00\x00\x12\x34\x56\x78""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08"
          "\xfe\x00\x00\x00\x12\x34\x56\x78""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x01\x02\x03\x04\x05\x06\x07\x08"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(table_stats_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const table_id = std::uint8_t{proto::OFPTT_ALL};
        auto const active_count = std::uint32_t{43};
        auto const lookup_count = std::uint32_t{83232};
        auto const matched_count = std::uint32_t{4323};

        auto const sut = multipart::table_stats{
            table_id, active_count, lookup_count, matched_count
        };

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_table_stats));
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.active_count() == active_count);
        BOOST_TEST(sut.lookup_count() == lookup_count);
        BOOST_TEST(sut.matched_count() == matched_count);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, table_stats_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.active_count() == sut.active_count());
        BOOST_TEST(copy.lookup_count() == sut.lookup_count());
        BOOST_TEST(copy.matched_count() == sut.matched_count());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, table_stats_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.table_id() == sut.table_id());
        BOOST_TEST(copy.active_count() == sut.active_count());
        BOOST_TEST(copy.lookup_count() == sut.lookup_count());
        BOOST_TEST(copy.matched_count() == sut.matched_count());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, table_stats_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_table_stats, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, table_stats_fixture)
    {
        auto it = bin_table_stats.begin();
        auto const it_end = bin_table_stats.end();

        auto const table_stats = multipart::table_stats::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(table_stats.length() == sut.length());
        BOOST_TEST(table_stats.table_id() == sut.table_id());
        BOOST_TEST(table_stats.active_count() == sut.active_count());
        BOOST_TEST(table_stats.lookup_count() == sut.lookup_count());
        BOOST_TEST(table_stats.matched_count() == sut.matched_count());
    }

BOOST_AUTO_TEST_SUITE_END() // table_stats_test


BOOST_AUTO_TEST_SUITE(table_stats_request_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = multipart::table_stats_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_TABLE);
        BOOST_TEST(sut.flags() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, table_stats_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, table_stats_request_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(src.length() == sut.length());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, table_stats_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_table_stats_request, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, table_stats_request_fixture)
    {
        auto it = bin_table_stats_request.begin();
        auto const it_end = bin_table_stats_request.end();

        auto const table_stats_request
            = multipart::table_stats_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(table_stats_request.version() == sut.version());
        BOOST_TEST(table_stats_request.type() == sut.type());
        BOOST_TEST(table_stats_request.length() == sut.length());
        BOOST_TEST(table_stats_request.xid() == sut.xid());
        BOOST_TEST(table_stats_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(table_stats_request.flags() == sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // table_stats_request_test


BOOST_AUTO_TEST_SUITE(table_stats_reply_test)

    BOOST_FIXTURE_TEST_CASE(construct_test, table_stats_fixture)
    {
        auto const size = 5;
        auto const table_stats = std::vector<multipart::table_stats>(
                size, table_stats_fixture::sut);

        auto const sut = multipart::table_stats_reply{table_stats};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_table_stats) * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_TABLE);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(construct_with_flag_test, table_stats_fixture)
    {
        auto const size = 0;
        auto const table_stats = std::vector<multipart::table_stats>(
                size, table_stats_fixture::sut);
        auto const flags = std::uint16_t(proto::OFPMPF_REPLY_MORE);

        auto const sut = multipart::table_stats_reply{table_stats, flags};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_table_stats) * size);
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_TABLE);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.size() == size);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, table_stats_reply_fixture)
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

    BOOST_FIXTURE_TEST_CASE(move_construct_test, table_stats_reply_fixture)
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

    BOOST_FIXTURE_TEST_CASE(encode_test, table_stats_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_table_stats_reply, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, table_stats_reply_fixture)
    {
        auto it = bin_table_stats_reply.begin();
        auto const it_end = bin_table_stats_reply.end();

        auto const table_stats_reply
            = multipart::table_stats_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(table_stats_reply.version() == sut.version());
        BOOST_TEST(table_stats_reply.type() == sut.type());
        BOOST_TEST(table_stats_reply.length() == sut.length());
        BOOST_TEST(table_stats_reply.xid() == sut.xid());
        BOOST_TEST(table_stats_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(table_stats_reply.flags() == sut.flags());
        BOOST_TEST(table_stats_reply.size() == sut.size());
    }

BOOST_AUTO_TEST_SUITE_END() // table_stats_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
