#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/barrier.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(barrier_test)

BOOST_AUTO_TEST_SUITE(barrier_request_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = barrier_request{};

        BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), OFPT_BARRIER_REQUEST);
        BOOST_CHECK_EQUAL(sut.length(), 8);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = barrier_request{};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.xid(), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture {
    barrier_request const sut{};
    std::vector<std::uint8_t> buffer{};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        sut.encode(buffer);
        auto it = buffer.begin();

        auto const decoded_message = barrier_request::decode(it, buffer.end());
        BOOST_CHECK(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // barrier_request_test


BOOST_AUTO_TEST_SUITE(barrier_reply_test)

struct barrier_request_fixture {
    barrier_request request{};
};
BOOST_FIXTURE_TEST_SUITE(instantiation_test, barrier_request_fixture)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = barrier_reply{request};

        BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), OFPT_BARRIER_REPLY);
        BOOST_CHECK_EQUAL(sut.length(), 8);
        BOOST_CHECK_EQUAL(sut.xid(), request.xid());
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = barrier_reply{request};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.xid(), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture : barrier_request_fixture {
    barrier_reply sut{request};
    std::vector<std::uint8_t> buffer{};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        sut.encode(buffer);
        auto it = buffer.begin();

        auto const decoded_message = barrier_reply::decode(it, buffer.end());
        BOOST_CHECK(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_fixture

BOOST_AUTO_TEST_SUITE_END() // barrier_reply_test

BOOST_AUTO_TEST_SUITE_END() // barrier_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
