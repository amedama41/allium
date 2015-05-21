#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/echo.hpp>
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <canard/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(echo_test)


struct echo_request_fixture
{
    echo_request sut{};
};
BOOST_FIXTURE_TEST_SUITE(echo_request_test, echo_request_fixture)
BOOST_AUTO_TEST_CASE(constructor_test)
{
    BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
    BOOST_CHECK_EQUAL(sut.type(), OFPT_ECHO_REQUEST);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header));
    // BOOST_CHECK_EQUAL(sut.xid(), 0);
}
BOOST_AUTO_TEST_CASE(lvalue_create_reply_test)
{
    auto reply = sut.reply();
    BOOST_CHECK_EQUAL(reply.version(), sut.version());
    BOOST_CHECK_EQUAL(reply.type(), OFPT_ECHO_REPLY);
    BOOST_CHECK_EQUAL(reply.length(), sut.length());
    BOOST_CHECK_EQUAL(reply.xid(), sut.xid());
    BOOST_CHECK_EQUAL(sut.data().size(), 0);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header));
}
BOOST_AUTO_TEST_CASE(rvalue_create_reply_test)
{
    auto reply = std::move(sut).reply();
    BOOST_CHECK_EQUAL(reply.version(), sut.version());
    BOOST_CHECK_EQUAL(reply.type(), OFPT_ECHO_REPLY);
    BOOST_CHECK_EQUAL(reply.length(), sut.length());
    BOOST_CHECK_EQUAL(reply.xid(), sut.xid());
    BOOST_CHECK_EQUAL(sut.data().size(), 0);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header));
}
struct buffer_fixture
{
    echo_request sut{};
    std::vector<unsigned char> buffer = sut.encode();
};
BOOST_FIXTURE_TEST_SUITE(buffer_test, buffer_fixture)
    BOOST_AUTO_TEST_CASE(buffer_size_test)
    {
        BOOST_CHECK_EQUAL(buffer.size(), sizeof(v13_detail::ofp_header));
    }
    BOOST_AUTO_TEST_CASE(version_test)
    {
        std::uint8_t version;
        std::memcpy(&version, buffer.data(), sizeof(version));
        BOOST_CHECK_EQUAL(canard::ntoh(version), sut.version());
    }
    BOOST_AUTO_TEST_CASE(type_test)
    {
        std::uint8_t type;
        std::memcpy(&type, buffer.data() + offsetof(v13_detail::ofp_header, type), sizeof(type));
        BOOST_CHECK_EQUAL(canard::ntoh(type), sut.type());
    }
    BOOST_AUTO_TEST_CASE(length_test)
    {
        std::uint16_t length;
        std::memcpy(&length, buffer.data() + offsetof(v13_detail::ofp_header, length), sizeof(length));
        BOOST_CHECK_EQUAL(canard::ntoh(length), buffer.size());
        BOOST_CHECK_EQUAL(canard::ntoh(length), sut.length());
    }
    BOOST_AUTO_TEST_CASE(xid_test)
    {
        std::uint32_t xid;
        std::memcpy(&xid, buffer.data() + offsetof(v13_detail::ofp_header, xid), sizeof(xid));
        BOOST_CHECK_EQUAL(canard::ntoh(xid), sut.xid());
    }
BOOST_AUTO_TEST_SUITE_END() // buffer_test

struct has_data_echo_request_fixture
{
    echo_request sut{{'A', 'B', 'C', 'D', 'E', 'F', 'G'}};
};
BOOST_FIXTURE_TEST_SUITE(has_data_echo_request_test, has_data_echo_request_fixture)
    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), OFPT_ECHO_REQUEST);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header) + 7);
        // BOOST_CHECK_EQUAL(sut.xid(), 0);
    }
    BOOST_AUTO_TEST_CASE(lvalue_create_reply_test)
    {
        auto reply = sut.reply();
        BOOST_CHECK_EQUAL(reply.version(), sut.version());
        BOOST_CHECK_EQUAL(reply.type(), OFPT_ECHO_REPLY);
        BOOST_CHECK_EQUAL(reply.length(), sut.length());
        BOOST_CHECK_EQUAL(reply.xid(), sut.xid());
        BOOST_CHECK_EQUAL(sut.data().size(), 7);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header) + 7);
    }
    BOOST_AUTO_TEST_CASE(rvalue_create_reply_test)
    {
        auto reply = std::move(sut).reply();
        BOOST_CHECK_EQUAL(reply.version(), sut.version());
        BOOST_CHECK_EQUAL(reply.type(), OFPT_ECHO_REPLY);
        BOOST_CHECK_EQUAL(reply.length(), sizeof(v13_detail::ofp_header) + 7);
        BOOST_CHECK_EQUAL(reply.xid(), sut.xid());
        BOOST_CHECK_EQUAL(sut.data().size(), 0);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header));
    }

    BOOST_AUTO_TEST_CASE(encode_decode_test)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
        BOOST_CHECK_EQUAL_COLLECTIONS(buffer.data() + 8, buffer.data() + buffer.size(), sut.data().begin(), sut.data().end());

        auto it = buffer.begin();
        auto const decoded_message = echo_request::decode(it, buffer.end());

        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
        CANARD_CHECK_EQUAL_COLLECTIONS(decoded_message.data(), sut.data());
    }
BOOST_AUTO_TEST_SUITE_END() // has_data_echo_request_test

BOOST_AUTO_TEST_SUITE_END() // echo_request_test


BOOST_AUTO_TEST_SUITE(echo_reply_test)

struct echo_reply_fixture
{
    echo_reply sut{};
};
BOOST_FIXTURE_TEST_CASE(constructor_test, echo_reply_fixture)
{
    BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
    BOOST_CHECK_EQUAL(sut.type(), OFPT_ECHO_REPLY);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_header));
    // BOOST_CHECK_EQUAL(sut.xid(), 0);
}

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = echo_reply{{'A', 'B', 'C', 'D', 'E', 'F'}};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    BOOST_CHECK_EQUAL_COLLECTIONS(buffer.data() + 8, buffer.data() + buffer.size(), sut.data().begin(), sut.data().end());

    auto it = buffer.begin();
    auto const decoded_message = echo_reply::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
    BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
    CANARD_CHECK_EQUAL_COLLECTIONS(decoded_message.data(), sut.data());
}

BOOST_AUTO_TEST_SUITE_END() // echo_reply_test


BOOST_AUTO_TEST_SUITE_END() // echo_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

