#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/echo.hpp>
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <canard/unit_test.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(echo_request_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::messages::echo_request{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_xid_test)
    {
        auto const xid = 0xffffffff;

        auto const sut = v13::messages::echo_request{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_data_test)
    {
        auto const bin = "\x01\x02\x03\x04"_bin;

        auto const sut = v13::messages::echo_request{of::binary_data{bin}};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(construct_from_data_and_xid_test)
    {
        auto const bin = "\xff\xff\xff\xff"_bin;
        auto const xid = 0xfefefefe;

        auto const sut = v13::messages::echo_request{of::binary_data{bin}, xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REQUEST);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});
    }


    struct echo_request_fixture
    {
        std::vector<std::uint8_t> bin = "\x00\xff\x01\xfe"_bin;
        v13::messages::echo_request sut{
            of::binary_data{bin}, 0x0013fffe
        };
        std::vector<std::uint8_t> const echo_request_bin
            = "\x04\x02\x00\x0c\x00\x13\xff\xfe" "\x00\xff\x01\xfe"_bin;
    };

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, echo_request_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, echo_request_fixture)
    {
        auto src = sut;

        auto copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(src.data_length() == 0);
        BOOST_TEST(src.data().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, echo_request_fixture)
    {
        auto copy = v13::messages::echo_request{};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, echo_request_fixture)
    {
        auto src = sut;
        auto copy = v13::messages::echo_request{};

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(src.data_length() == 0);
        BOOST_TEST(src.data().empty());
    }

    BOOST_FIXTURE_TEST_CASE(extract_data_test, echo_request_fixture)
    {
        auto const data = sut.extract_data();

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());

        auto const data_range
            = boost::make_iterator_range(data.begin(), data.end());
        BOOST_TEST(data_range == bin, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, echo_request_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == echo_request_bin);
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, echo_request_fixture)
    {
        auto it = echo_request_bin.begin();
        auto it_end = echo_request_bin.end();

        auto const echo = v13::messages::echo_request::decode(it, it_end);

        BOOST_TEST((it == it_end));

        BOOST_TEST(echo.version() == sut.version());
        BOOST_TEST(echo.type() == sut.type());
        BOOST_TEST(echo.length() == sut.length());
        BOOST_TEST(echo.xid() == sut.xid());
        BOOST_TEST(echo.data_length() == sut.data_length());
        BOOST_TEST((echo.data() == sut.data()));
    }

    struct no_data_echo_request_fixture
    {
        v13::messages::echo_request sut{0x0013fffe};
        std::vector<std::uint8_t> const echo_request_bin
            = "\x04\x02\x00\x08\x00\x13\xff\xfe"_bin;
    };

    BOOST_FIXTURE_TEST_CASE(no_data_encode_test, no_data_echo_request_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == echo_request_bin);
    }

    BOOST_FIXTURE_TEST_CASE(no_data_decode_test, no_data_echo_request_fixture)
    {
        auto it = echo_request_bin.begin();
        auto it_end = echo_request_bin.end();

        auto const echo = v13::messages::echo_request::decode(it, it_end);

        BOOST_TEST((it == it_end));

        BOOST_TEST(echo.version() == sut.version());
        BOOST_TEST(echo.type() == sut.type());
        BOOST_TEST(echo.length() == sut.length());
        BOOST_TEST(echo.xid() == sut.xid());
        BOOST_TEST(echo.data_length() == 0);
        BOOST_TEST(echo.data().empty());
    }

BOOST_AUTO_TEST_SUITE_END() // echo_request_test


BOOST_AUTO_TEST_SUITE(echo_reply_test)

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::messages::echo_reply{};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_xid_test)
    {
        auto const xid = 0xffffffff;

        auto const sut = v13::messages::echo_reply{xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_data_test)
    {
        auto const bin = "\x01\x02\x03\x04"_bin;

        auto const sut = v13::messages::echo_reply{of::binary_data{bin}};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(construct_from_data_and_xid_test)
    {
        auto const bin = "\xff\xff\xff\xff"_bin;
        auto const xid = 0xfefefefe;

        auto const sut = v13::messages::echo_reply{of::binary_data{bin}, xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(construct_from_lvalue_echo_request_test)
    {
        auto const bin = "\xff\xff\xff\xff"_bin;
        auto const xid = 0xfefefefe;
        auto const request
            = v13::messages::echo_request{of::binary_data{bin}, xid};

        auto const sut = v13::messages::echo_reply{request};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(construct_from_rvalue_echo_request_test)
    {
        auto const bin = "\xff\xff\xff\xff"_bin;
        auto const xid = 0xfefefefe;
        auto request = v13::messages::echo_request{of::binary_data{bin}, xid};

        auto const sut = v13::messages::echo_reply{std::move(request)};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_ECHO_REPLY);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header) + bin.size());
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.data_length() == bin.size());
        BOOST_TEST(sut.data() == bin, boost::test_tools::per_element{});

        BOOST_TEST(request.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(request.data_length() == 0);
        BOOST_TEST(request.data().empty());
    }

    struct echo_reply_fixture
    {
        std::vector<std::uint8_t> bin = "\x00\xff\x01\xfe"_bin;
        v13::messages::echo_reply sut{
            of::binary_data{bin}, 0x0013fffe
        };
        std::vector<std::uint8_t> const echo_reply_bin
            = "\x04\x03\x00\x0c\x00\x13\xff\xfe" "\x00\xff\x01\xfe"_bin;
    };

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, echo_reply_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, echo_reply_fixture)
    {
        auto src = sut;

        auto copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(src.data_length() == 0);
        BOOST_TEST(src.data().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, echo_reply_fixture)
    {
        auto copy = v13::messages::echo_reply{};

        copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, echo_reply_fixture)
    {
        auto src = sut;
        auto copy = v13::messages::echo_reply{};

        copy = std::move(src);

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.data_length() == sut.data_length());
        BOOST_TEST((copy.data() == sut.data()));

        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(src.data_length() == 0);
        BOOST_TEST(src.data().empty());
    }

    BOOST_FIXTURE_TEST_CASE(extract_data_test, echo_reply_fixture)
    {
        auto const data = sut.extract_data();

        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_header));
        BOOST_TEST(sut.data_length() == 0);
        BOOST_TEST(sut.data().empty());

        auto const data_range
            = boost::make_iterator_range(data.begin(), data.end());
        BOOST_TEST(data_range == bin, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, echo_reply_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == echo_reply_bin);
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, echo_reply_fixture)
    {
        auto it = echo_reply_bin.begin();
        auto it_end = echo_reply_bin.end();

        auto const echo = v13::messages::echo_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));

        BOOST_TEST(echo.version() == sut.version());
        BOOST_TEST(echo.type() == sut.type());
        BOOST_TEST(echo.length() == sut.length());
        BOOST_TEST(echo.xid() == sut.xid());
        BOOST_TEST(echo.data_length() == sut.data_length());
        BOOST_TEST((echo.data() == sut.data()));
    }

    struct no_data_echo_reply_fixture
    {
        v13::messages::echo_reply sut{0x0013fffe};
        std::vector<std::uint8_t> const echo_reply_bin
            = "\x04\x03\x00\x08\x00\x13\xff\xfe"_bin;
    };

    BOOST_FIXTURE_TEST_CASE(no_data_encode_test, no_data_echo_reply_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == echo_reply_bin);
    }

    BOOST_FIXTURE_TEST_CASE(no_data_decode_test, no_data_echo_reply_fixture)
    {
        auto it = echo_reply_bin.begin();
        auto it_end = echo_reply_bin.end();

        auto const echo = v13::messages::echo_reply::decode(it, it_end);

        BOOST_TEST((it == it_end));

        BOOST_TEST(echo.version() == sut.version());
        BOOST_TEST(echo.type() == sut.type());
        BOOST_TEST(echo.length() == sut.length());
        BOOST_TEST(echo.xid() == sut.xid());
        BOOST_TEST(echo.data_length() == 0);
        BOOST_TEST(echo.data().empty());
    }


BOOST_AUTO_TEST_SUITE_END() // echo_reply_test


BOOST_AUTO_TEST_SUITE_END() // message_test

