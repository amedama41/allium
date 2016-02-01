#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/switch_config.hpp>
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

BOOST_AUTO_TEST_SUITE(get_config_request_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = v13::messages::get_config_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const xid = std::uint32_t{79};

        auto const sut = v13::messages::get_config_request{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto sut = v13::messages::get_config_request{0x40};

        sut.encode(buffer);

        char const expected[] = "\x04\x07\00\x08\x00\x00\x00\x40";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[] = "\x04\x07\00\x08\x00\x00\x00\x80";

        auto it = buffer;
        auto it_end = buffer + sizeof(buffer) - 1;
        auto sut = v13::messages::get_config_request::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == 0x80);
    }

BOOST_AUTO_TEST_SUITE_END() // get_config_request_test


BOOST_AUTO_TEST_SUITE(get_config_reply_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const flags = proto::OFPC_FRAG_NORMAL;
        auto const miss_send_len = std::uint16_t{proto::OFPCML_MAX};
        auto const xid = std::uint32_t{0x79};

        auto const sut
            = v13::messages::get_config_reply{flags, miss_send_len, xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_config));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.miss_send_length() == miss_send_len);
    }

    BOOST_AUTO_TEST_CASE(constructor_from_request_test)
    {
        auto const request = v13::messages::get_config_request{0x3382};
        auto const flags = proto::OFPC_FRAG_NORMAL;
        auto const miss_send_len = std::uint16_t{0x3456};

        auto const sut
            = v13::messages::get_config_reply{request, flags, miss_send_len};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_config));
        BOOST_TEST(sut.xid() == request.xid());
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.miss_send_length() == miss_send_len);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto const sut = v13::messages::get_config_reply{
              v13::messages::get_config_request{0x8}
            , proto::OFPC_FRAG_DROP, 0xff07
        };

        sut.encode(buffer);

        char const expected[]
            = "\x04\x08\x00\x0c\x00\x00\x00\x08"
              "\x00\x01\xff\x07";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[]
            = "\x04\x08\x00\x0c\x00\x01\x02\x03"
              "\x00\x02\xff\xff";

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer) - 1;
        auto const sut = v13::messages::get_config_reply::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_GET_CONFIG_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_config));
        BOOST_TEST(sut.xid() == 0x10203);
        BOOST_TEST(sut.flags() == proto::OFPC_FRAG_REASM);
        BOOST_TEST(sut.miss_send_length() == proto::OFPCML_NO_BUFFER);
    }

BOOST_AUTO_TEST_SUITE_END() // get_config_reply_test


BOOST_AUTO_TEST_SUITE(set_config_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const flags = proto::OFPC_FRAG_NORMAL;
        auto const miss_send_len = std::uint16_t{proto::OFPCML_MAX};
        auto const xid = std::uint32_t{0x79};

        auto const sut
            = v13::messages::set_config{flags, miss_send_len, xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_SET_CONFIG);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_config));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.flags() == flags);
        BOOST_TEST(sut.miss_send_length() == miss_send_len);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto const sut
            = v13::messages::set_config{proto::OFPC_FRAG_DROP, 0xff07, 0x8};

        sut.encode(buffer);

        char const expected[]
            = "\x04\x09\x00\x0c\x00\x00\x00\x08"
              "\x00\x01\xff\x07";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[]
            = "\x04\x09\x00\x0c\x00\x01\x02\x03"
              "\x00\x02\xff\xff";

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer) - 1;
        auto sut = v13::messages::set_config::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_SET_CONFIG);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_switch_config));
        BOOST_TEST(sut.xid() == 0x10203);
        BOOST_TEST(sut.flags() == proto::OFPC_FRAG_REASM);
        BOOST_TEST(sut.miss_send_length() == proto::OFPCML_NO_BUFFER);
    }


BOOST_AUTO_TEST_SUITE_END() // set_config_test

BOOST_AUTO_TEST_SUITE_END() // message_test
