#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/multipart_message/description.hpp>
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

struct description_request_fixture
{
    multipart::description_request sut{0x12345678};
    std::vector<std::uint8_t> bin_description_request
        = "\x04\x12\x00\x10\x12\x34\x56\x78""\x00\x00\x00\x00\x00\x00\x00\x00"_bin
        ;
};

struct description_reply_fixture
{
    multipart::description_reply sut{
          "manufacture_desc"
        , "hardware_desc"
        , "software_desc"
        , "serial_number"
        , "datapath_desc"
        , 0x12345678
    };
    std::vector<std::uint8_t> bin_description_reply
        = "\x04\x13\x04\x30\x12\x34\x56\x78""\x00\x00\x00\x00\x00\x00\x00\x00"
          "m" "a" "n" "u" "f" "a" "c" "t"   "u" "r" "e" "_" "d" "e" "s" "c"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "h" "a" "r" "d" "w" "a" "r" "e"   "_" "d" "e" "s" "c" "\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "s" "o" "f" "t" "w" "a" "r" "e"   "_" "d" "e" "s" "c" "\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "s" "e" "r" "i" "a" "l" "_" "n"   "u" "m" "b" "e" "r" "\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "d" "a" "t" "a" "p" "a" "t" "h"   "_" "d" "e" "s" "c" "\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"
          "\x00\x00\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x00\x00\x00\x00\x00"_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(message_test)
BOOST_AUTO_TEST_SUITE(multipart_test)

BOOST_AUTO_TEST_SUITE(description_request_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = multipart::description_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_request));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_DESC);
        BOOST_TEST(sut.flags() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, description_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, description_request_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_description_request, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, description_request_fixture)
    {
        auto it = bin_description_request.begin();
        auto const it_end = bin_description_request.end();

        auto const description_request
            = multipart::description_request::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(description_request.version() == sut.version());
        BOOST_TEST(description_request.type() == sut.type());
        BOOST_TEST(description_request.length() == sut.length());
        BOOST_TEST(description_request.xid() == sut.xid());
        BOOST_TEST(description_request.multipart_type() == sut.multipart_type());
        BOOST_TEST(description_request.flags() == sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // description_request_test


BOOST_AUTO_TEST_SUITE(description_reply_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = multipart::description_reply{
              "manufacture_desc"
            , "hardware_desc"
            , "software_desc"
            , "serial_number"
            , "datapath_desc"
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_MULTIPART_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_multipart_reply)
                                 + sizeof(v13_detail::ofp_desc));
        BOOST_TEST(sut.multipart_type() == proto::OFPMP_DESC);
        BOOST_TEST(sut.flags() == 0);
        BOOST_TEST(sut.manufacture_desc() == "manufacture_desc");
        BOOST_TEST(sut.hardware_desc() == "hardware_desc");
        BOOST_TEST(sut.software_desc() == "software_desc");
        BOOST_TEST(sut.serial_number() == "serial_number");
        BOOST_TEST(sut.datapath_desc() == "datapath_desc");
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, description_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.multipart_type() == sut.multipart_type());
        BOOST_TEST(copy.flags() == sut.flags());
        BOOST_TEST(copy.manufacture_desc() == sut.manufacture_desc());
        BOOST_TEST(copy.hardware_desc() == sut.hardware_desc());
        BOOST_TEST(copy.software_desc() == sut.software_desc());
        BOOST_TEST(copy.serial_number() == sut.serial_number());
        BOOST_TEST(copy.datapath_desc() == sut.datapath_desc());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, description_reply_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_description_reply, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, description_reply_fixture)
    {
        auto it = bin_description_reply.begin();
        auto const it_end = bin_description_reply.end();

        auto const description_reply
            = multipart::description_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(description_reply.version() == sut.version());
        BOOST_TEST(description_reply.type() == sut.type());
        BOOST_TEST(description_reply.length() == sut.length());
        BOOST_TEST(description_reply.multipart_type() == sut.multipart_type());
        BOOST_TEST(description_reply.flags() == sut.flags());
        BOOST_TEST(description_reply.manufacture_desc() == sut.manufacture_desc());
        BOOST_TEST(description_reply.hardware_desc() == sut.hardware_desc());
        BOOST_TEST(description_reply.software_desc() == sut.software_desc());
        BOOST_TEST(description_reply.serial_number() == sut.serial_number());
        BOOST_TEST(description_reply.datapath_desc() == sut.datapath_desc());
    }

BOOST_AUTO_TEST_SUITE_END() // description_reply_test

BOOST_AUTO_TEST_SUITE_END() // multipart_test
BOOST_AUTO_TEST_SUITE_END() // message_test
