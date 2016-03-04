#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/barrier.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = canard::network::openflow::v13;
using proto = v13::protocol;

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(barrier_request_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::messages::barrier_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const xid = std::uint32_t{0xffffffff};
        auto const sut = v13::messages::barrier_request{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
    }

    BOOST_AUTO_TEST_CASE(move_construct_test)
    {
        auto sut = v13::messages::barrier_request{0x0000ffff};

        auto const copy = std::move(sut);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto const sut = v13::messages::barrier_request{0x12345678};
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        auto const expected = "\x04\x14\x00\x08\x12\x34\x56\x78"_bin;
        BOOST_TEST(buffer == expected, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        auto const buffer
            = std::string("\x04\x14\x00\x08\x01\x02\x03\x04", 8);

        auto it = buffer.begin();
        auto const it_end = buffer.end();
        auto const sut
            = v13::messages::barrier_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == 0x01020304);
    }

BOOST_AUTO_TEST_SUITE_END() // barrier_request_test


BOOST_AUTO_TEST_SUITE(barrier_reply_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::messages::barrier_reply{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
    }

    BOOST_AUTO_TEST_CASE(construct_from_xid)
    {
        auto const xid = std::uint32_t{0x00110022};
        auto const sut = v13::messages::barrier_reply{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
    }

    BOOST_AUTO_TEST_CASE(construct_from_request_test)
    {
        auto const request = v13::messages::barrier_request{0x0abcdef0};
        auto const sut = v13::messages::barrier_reply{request};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == request.xid());
    }

    BOOST_AUTO_TEST_CASE(move_construct_test)
    {
        auto sut = v13::messages::barrier_reply{0xffffffff};

        auto const copy = std::move(sut);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto const sut = v13::messages::barrier_reply{0xff12ff34};
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        auto const expected = "\x04\x15\x00\x08\xff\x12\xff\x34"_bin;
        BOOST_TEST(buffer == expected, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        auto const buffer = "\x04\x15\x00\x08\x10\x20\x30\x40"_bin;

        auto it = buffer.begin();
        auto it_end = buffer.end();
        auto const sut = v13::messages::barrier_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_BARRIER_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == 0x10203040);
    }

BOOST_AUTO_TEST_SUITE_END() // barrier_reply_test

BOOST_AUTO_TEST_SUITE_END() // message_test

