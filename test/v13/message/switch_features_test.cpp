#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/message/switch_features.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace of = canard::network::openflow;
namespace v13 = canard::network::openflow::v13;
using proto = v13::protocol;

template <std::size_t N>
static auto to_buffer(char const (&expected)[N])
    -> std::vector<unsigned char>
{
    return std::vector<unsigned char>(expected, expected + N - 1);
}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(features_request_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto sut = v13::messages::features_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FEATURES_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const xid = std::uint32_t{32};

        auto sut = v13::messages::features_request{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FEATURES_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto sut = v13::messages::features_request{64};

        sut.encode(buffer);

        char const expected[] = "\x04\x05\00\x08\x00\x00\x00\x40";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[] = "\x04\x05\00\x08\x00\x00\x00\x80";

        auto it = buffer;
        auto it_end = buffer + sizeof(buffer) - 1;
        auto sut = v13::messages::features_request::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FEATURES_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == 128);
    }

BOOST_AUTO_TEST_SUITE_END() // features_request_test


BOOST_AUTO_TEST_SUITE(features_reply_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const request = v13::messages::features_request{16};
        auto const dpid = std::uint64_t{4};
        auto const n_buffers = std::uint32_t{9};
        auto const n_tables = std::uint32_t{8};
        auto const auxiliary_id = std::uint8_t{12};
        auto const capabilities
            = proto::OFPC_FLOW_STATS | proto::OFPC_PORT_STATS;

        auto sut = v13::messages::features_reply{
            request, dpid, n_buffers, n_tables, auxiliary_id, capabilities
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FEATURES_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_features));
        BOOST_TEST(sut.xid() == request.xid());
        BOOST_TEST(sut.datapath_id() == dpid);
        BOOST_TEST(sut.num_buffers() == n_buffers);
        BOOST_TEST(sut.num_tables() == n_tables);
        BOOST_TEST(sut.auxiliary_id() == auxiliary_id);
        BOOST_TEST(sut.capabilities() == capabilities);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto sut = v13::messages::features_reply{
              v13::messages::features_request{8}
            , 5, 7, 10, 11, proto::OFPC_TABLE_STATS | proto::OFPC_PORT_BLOCKED
        };

        sut.encode(buffer);

        char const expected[]
            = "\x04\x06\x00\x20\x00\x00\x00\x08"
              "\x00\x00\x00\x00\x00\x00\x00\x05"
              "\x00\x00\x00\x07\x0a\x0b\x00\x00"
              "\x00\x00\x01\x02\x00\x00\x00\x00";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[]
            = "\x04\x06\x00\x20\x00\x01\x02\x03"
              "\x00\x00\x00\x12\x00\x34\x00\x56"
              "\x00\x01\xff\x07\x03\x10\x00\x00"
              "\x00\x00\x01\x02\x00\x00\x00\x00";

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer) - 1;
        auto sut = v13::messages::features_reply::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_FEATURES_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_features));
        BOOST_TEST(sut.xid() == 0x10203);
        BOOST_TEST(sut.datapath_id() == 0x1200340056);
        BOOST_TEST(sut.num_buffers() == 0x1ff07);
        BOOST_TEST(sut.num_tables() == 0x3);
        BOOST_TEST(sut.auxiliary_id() == 0x10);
        BOOST_TEST(sut.capabilities() == 0x102);
    }

BOOST_AUTO_TEST_SUITE_END() // features_reply_test

BOOST_AUTO_TEST_SUITE_END() // message_test
