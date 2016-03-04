#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/port_description.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "../../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace multipart = v13::messages::multipart;
using proto = v13::protocol;

namespace {

struct port_description_request_fixture
{
    multipart::port_description_request sut{0x12345678};
    std::vector<std::uint8_t> binary
        = "\x04\x12\x00\x10\x12\x34\x56\x78""\x00\x0d\x00\x00\x00\x00\x00\x00"
          ""_bin
        ;
};

struct port_fixture
{
    v13::port port1 = v13::port::from_ofp_port({
              0x1
            , {0}, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}, {0}, "eth1"
            , proto::OFPPC_PORT_DOWN
            , proto::OFPPS_LINK_DOWN
            , proto::OFPPF_100GB_FD | proto::OFPPF_FIBER
            , proto::OFPPF_10MB_HD | proto::OFPPF_100MB_HD | proto::OFPPF_100GB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_10MB_HD | proto::OFPPF_100MB_HD | proto::OFPPF_100GB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_10MB_FD | proto::OFPPF_100MB_FD | proto::OFPPF_100GB_FD | proto::OFPPF_AUTONEG
            , 0xf1f2f3f4, 0xf5f6f7f8
    });
    v13::port port2 = v13::port::from_ofp_port({
              0x2
            , {0}, {0x11, 0x12, 0x13, 0x14, 0x15, 0x16}, {0}, "eth2"
            , 0
            , 0
            , proto::OFPPF_100MB_FD | proto::OFPPF_COPPER
            , proto::OFPPF_10MB_FD | proto::OFPPF_100MB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_10MB_FD | proto::OFPPF_100MB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_100MB_FD | proto::OFPPF_100GB_FD | proto::OFPPF_AUTONEG
            , 0xe1e2e3e4, 0xe5e6e7e8
    });
    v13::port port3 = v13::port::from_ofp_port({
              proto::OFPP_MAX
            , {0}, {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6}, {0}, "eth100"
            , proto::OFPPC_NO_RECV | proto::OFPPC_NO_PACKET_IN
            , proto::OFPPS_BLOCKED | proto::OFPPS_LIVE
            , proto::OFPPF_1TB_FD | proto::OFPPF_FIBER
            , proto::OFPPF_100GB_FD | proto::OFPPF_1TB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_100GB_FD | proto::OFPPF_1TB_FD | proto::OFPPF_AUTONEG
            , proto::OFPPF_40GB_FD | proto::OFPPF_1TB_FD | proto::OFPPF_AUTONEG
            , 0x01020304, 0x05060708
    });
};

struct port_description_reply_fixture : port_fixture
{
    multipart::port_description_reply sut{
        {port1, port2, port3}, proto::OFPMPF_REPLY_MORE, 0x12345678
    };
    std::vector<std::uint8_t> binary
        = "\x04\x13\x00\xd0\x12\x34\x56\x78""\x00\x0d\x00\x01\x00\x00\x00\x00"
          "\x00\x00\x00\x01\x00\x00\x00\x00""\x01\x02\x03\x04\x05\x06\x00\x00"
          "e" "t" "h" "1" "\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x01\x00\x00\x00\x01""\x00\x00\x11\x00\x00\x00\x21\x05"
          "\x00\x00\x21\x05\x00\x00\x21\x0a""\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8"
          "\x00\x00\x00\x02\x00\x00\x00\x00""\x11\x12\x13\x14\x15\x16\x00\x00"
          "e" "t" "h" "2" "\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x08\x08\x00\x00\x20\x0a"
          "\x00\x00\x20\x0a\x00\x00\x21\x08""\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8"
          "\xff\xff\xff\x00\x00\x00\x00\x00""\xf1\xf2\xf3\xf4\xf5\xf6\x00\x00"
          "e" "t" "h" "1" "0" "0" "\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x44\x00\x00\x00\x06""\x00\x00\x12\x00\x00\x00\x23\x00"
          "\x00\x00\x23\x00\x00\x00\x22\x80""\x01\x02\x03\x04\x05\x06\x07\x08"
          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(port_description_request_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = multipart::port_description_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_PORT_DESC);
        BOOST_TEST(sut.flags() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_description_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_description_request_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const port_description_request
            = multipart::port_description_request::decode(it, it_end);

        BOOST_TEST(port_description_request.version() == sut.version());
        BOOST_TEST(port_description_request.type() == sut.type());
        BOOST_TEST(port_description_request.length() == sut.length());
        BOOST_TEST(port_description_request.xid() == sut.xid());
        BOOST_TEST(port_description_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(port_description_request.flags() == sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // port_description_request_test


BOOST_AUTO_TEST_SUITE(port_description_reply_test)

    BOOST_FIXTURE_TEST_CASE(construct_test, port_fixture)
    {
        auto const ports = std::vector<v13::port>{port1, port2};

        auto const sut = multipart::port_description_reply{ports};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + v13::port::length() * ports.size());
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_PORT_DESC);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.size() == 2);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_description_reply_fixture)
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

    BOOST_FIXTURE_TEST_CASE(move_construct_test, port_description_reply_fixture)
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
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_description_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_description_reply_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const port_description_reply
            = multipart::port_description_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(port_description_reply.version() == sut.version());
        BOOST_TEST(port_description_reply.type() == sut.type());
        BOOST_TEST(port_description_reply.length() == sut.length());
        BOOST_TEST(port_description_reply.xid() == sut.xid());
        BOOST_TEST(port_description_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(port_description_reply.flags() == sut.flags());
        BOOST_TEST(port_description_reply.size() == sut.size());
    }

BOOST_AUTO_TEST_SUITE_END() // port_description_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
