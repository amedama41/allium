#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/message/error_msg.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <boost/endian/conversion.hpp>
#include <canard/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(error_msg_test)

struct error_data_fixture
{
    std::vector<std::uint8_t> data;
    error_data_fixture()
    {
        auto header = boost::endian::native_to_big(v13_detail::ofp_header{protocol::OFP_VERSION, protocol::OFPT_HELLO, 8, 50});
        data.assign(reinterpret_cast<unsigned char*>(&header), reinterpret_cast<unsigned char*>(&header + 1));
    }
};
BOOST_FIXTURE_TEST_SUITE(instantiation_test, error_data_fixture)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = error_msg{protocol::OFPET_HELLO_FAILED, protocol::OFPHFC_INCOMPATIBLE, data};

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_ERROR);
        BOOST_CHECK_EQUAL(sut.length(), 12 + data.size());
        BOOST_CHECK_EQUAL(sut.error_type(), protocol::OFPET_HELLO_FAILED);
        BOOST_CHECK_EQUAL(sut.error_code(), protocol::OFPHFC_INCOMPATIBLE);
        CANARD_CHECK_EQUAL_COLLECTIONS(sut.data(), data);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = error_msg{protocol::OFPET_HELLO_FAILED, protocol::OFPHFC_INCOMPATIBLE, data};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.xid(), sut.xid());
        CANARD_CHECK_EQUAL_COLLECTIONS(copy.data(), data);
        BOOST_CHECK_EQUAL(sut.data().size(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_FIXTURE_TEST_CASE(decoding_failed_request_header, error_data_fixture)
{
    auto const sut = error_msg{protocol::OFPET_BAD_REQUEST, protocol::OFPBRC_BAD_LEN, data};

    auto const header = sut.failed_request_header();

    BOOST_CHECK_EQUAL(header.version, protocol::OFP_VERSION);
    BOOST_CHECK_EQUAL(header.type, protocol::OFPT_HELLO);
    BOOST_CHECK_EQUAL(header.length, 8);
    BOOST_CHECK_EQUAL(header.xid, 50);
}

BOOST_FIXTURE_TEST_CASE(encode_decode_test, error_data_fixture)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = error_msg{protocol::OFPET_FLOW_MOD_FAILED, protocol::OFPFMFC_TABLE_FULL, data};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_message = error_msg::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
    BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
    CANARD_CHECK_EQUAL_COLLECTIONS(decoded_message.data(), sut.data());
}

BOOST_AUTO_TEST_SUITE_END() // error_msg_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
