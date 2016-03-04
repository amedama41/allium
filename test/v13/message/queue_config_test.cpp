#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/queue_config.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace queue_props = v13::queue_properties;
using proto = v13::protocol;

namespace {

struct queue_get_config_request_fixture
{
    v13::messages::queue_get_config_request sut{
        proto::OFPP_MAX, 0x12345678
    };
    std::vector<std::uint8_t> binary
        = "\x04\x16\x00\x10\x12\x34\x56\x78""\xff\xff\xff\x00\x00\x00\x00\x00"
          ""_bin
        ;
};

struct packet_queue_fixture
{
    v13::packet_queue queue1{
        0x1, 0x1, {
              v13::any_queue_property{queue_props::min_rate{0x1234}}
            , v13::any_queue_property{queue_props::max_rate{0x5432}}
        }
    };
    v13::packet_queue queue2{
        0x2, 0x1, {
              v13::any_queue_property{queue_props::max_rate{0x3212}}
        }
    };
    v13::packet_queue queue3{
        0xfffffffe, 0x1, {
              v13::any_queue_property{queue_props::max_rate{0xf0f0}}
            , v13::any_queue_property{queue_props::min_rate{0x0f0f}}
        }
    };
};

struct queue_get_config_reply_fixture : packet_queue_fixture
{
    v13::messages::queue_get_config_reply sut{
          0x1, {queue1, queue2, queue3}, 0x12345678
    };
    std::vector<std::uint8_t> binary
        = "\x04\x17\x00\x90\x12\x34\x56\x78""\x00\x00\x00\x01\x00\x00\x00\x00"

          "\x00\x00\x00\x01\x00\x00\x00\x01""\x00\x30\x00\x00\x00\x00\x00\x00"
          "\x00\x01\x00\x10\x00\x00\x00\x00""\x12\x34\x00\x00\x00\x00\x00\x00"
          "\x00\x02\x00\x10\x00\x00\x00\x00""\x54\x32\x00\x00\x00\x00\x00\x00"

          "\x00\x00\x00\x02\x00\x00\x00\x01""\x00\x20\x00\x00\x00\x00\x00\x00"
          "\x00\x02\x00\x10\x00\x00\x00\x00""\x32\x12\x00\x00\x00\x00\x00\x00"

          "\xff\xff\xff\xfe\x00\x00\x00\x01""\x00\x30\x00\x00\x00\x00\x00\x00"
          "\x00\x02\x00\x10\x00\x00\x00\x00""\xf0\xf0\x00\x00\x00\x00\x00\x00"
          "\x00\x01\x00\x10\x00\x00\x00\x00""\x0f\x0f\x00\x00\x00\x00\x00\x00"
          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(queue_get_config_request_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port_no = std::uint32_t{1};

        auto const sut = v13::messages::queue_get_config_request{port_no};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_QUEUE_GET_CONFIG_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_queue_get_config_request));
        BOOST_TEST(sut.port_no() == port_no);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, queue_get_config_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, queue_get_config_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, queue_get_config_request_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const queue_get_config_request
            = v13::messages::queue_get_config_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(queue_get_config_request.version() == sut.version());
        BOOST_TEST(queue_get_config_request.type() == sut.type());
        BOOST_TEST(queue_get_config_request.length() == sut.length());
        BOOST_TEST(queue_get_config_request.xid() == sut.xid());
        BOOST_TEST(queue_get_config_request.port_no() == sut.port_no());
    }

BOOST_AUTO_TEST_SUITE_END() // queue_get_config_request_test


BOOST_AUTO_TEST_SUITE(queue_get_config_reply_test)

    BOOST_FIXTURE_TEST_CASE(construct_test, packet_queue_fixture)
    {
        auto const port_no = std::uint32_t{proto::OFPP_MAX};

        auto const sut = v13::messages::queue_get_config_reply{
            port_no, {queue1, queue2}
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_QUEUE_GET_CONFIG_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_queue_get_config_reply)
                                 + queue1.length() + queue2.length());
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.queues().size() == 2);
        BOOST_TEST(sut.queues()[0].queue_id() == queue1.queue_id());
        BOOST_TEST(sut.queues()[1].queue_id() == queue2.queue_id());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, queue_get_config_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.queues().size() == sut.queues().size());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, queue_get_config_reply_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.queues().size() == sut.queues().size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_queue_get_config_reply));
        BOOST_TEST(src.queues().size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, queue_get_config_reply_fixture)
    {
        auto copy = v13::messages::queue_get_config_reply{0, {}};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.queues().size() == sut.queues().size());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, queue_get_config_reply_fixture)
    {
        auto copy = v13::messages::queue_get_config_reply{0, {}};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.queues().size() == sut.queues().size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_queue_get_config_reply));
        BOOST_TEST(src.queues().size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, queue_get_config_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, queue_get_config_reply_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const queue_get_config_reply
            = v13::messages::queue_get_config_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(queue_get_config_reply.version() == sut.version());
        BOOST_TEST(queue_get_config_reply.type() == sut.type());
        BOOST_TEST(queue_get_config_reply.length() == sut.length());
        BOOST_TEST(queue_get_config_reply.xid() == sut.xid());
        BOOST_TEST(queue_get_config_reply.port_no() == sut.port_no());
        BOOST_TEST(queue_get_config_reply.queues().size() == sut.queues().size());
    }

BOOST_AUTO_TEST_SUITE_END() // queue_get_config_reply_test

BOOST_AUTO_TEST_SUITE_END() // message_test
