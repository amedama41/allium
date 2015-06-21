#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/hello.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(hello_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto sut = hello{};
        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_HELLO);
        BOOST_CHECK_EQUAL(sut.length(), 8);
    }

    BOOST_AUTO_TEST_CASE(construct_by_uint32_vector_test)
    {
        auto sut = hello{std::vector<std::uint32_t>{0x00000012}};
        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_HELLO);
        BOOST_CHECK_EQUAL(sut.length(), 16);
    }

    BOOST_AUTO_TEST_CASE(construct_by_version_bitmap_vector_test)
    {
        auto sut = hello{{hello_elements::versionbitmap{{0x00000012}}, hello_elements::unknown_element{2}}};
        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_HELLO);
        BOOST_CHECK_EQUAL(sut.length(), 24);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = hello{{hello_elements::versionbitmap{{0x00000012}}, hello_elements::unknown_element{2}}};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.xid(), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(encode_test)

struct no_elems_buffers_fixture
{
    hello sut = hello{};
    std::vector<unsigned char> buffer = sut.encode();
};
BOOST_FIXTURE_TEST_SUITE(no_elems_buffers_test, no_elems_buffers_fixture)

    BOOST_AUTO_TEST_CASE(buffer_size_test)
    {
        BOOST_CHECK_EQUAL(buffer.size(), sizeof(v13_detail::ofp_hello));
    }

    BOOST_AUTO_TEST_CASE(version_test)
    {
        auto version = std::uint8_t{};
        std::memcpy(&version, buffer.data(), sizeof(version));
        BOOST_CHECK_EQUAL(canard::ntoh(version), protocol::OFP_VERSION);
    }

    BOOST_AUTO_TEST_CASE(type_test)
    {
        auto type = std::uint8_t{};
        std::memcpy(&type, buffer.data() + offsetof(v13_detail::ofp_header, type), sizeof(type));
        BOOST_CHECK_EQUAL(canard::ntoh(type), protocol::OFPT_HELLO);
    }

    BOOST_AUTO_TEST_CASE(length_test)
    {
        auto length = std::uint16_t{};
        std::memcpy(&length, buffer.data() + offsetof(v13_detail::ofp_header, length), sizeof(length));
        BOOST_CHECK_EQUAL(canard::ntoh(length), buffer.size());
    }

    BOOST_AUTO_TEST_CASE(xid_test)
    {
        auto xid = std::uint32_t{};
        std::memcpy(&xid, buffer.data() + offsetof(v13_detail::ofp_header, xid), sizeof(xid));
        BOOST_CHECK_EQUAL(canard::ntoh(xid), sut.xid());
    }

BOOST_AUTO_TEST_SUITE_END() // no_elems_buffers_test


struct versionbitmap_buffers_fixture
{
    std::vector<std::uint32_t> bitmaps{3, 4};
    hello sut{bitmaps};
    std::vector<unsigned char> buffer = sut.encode();
};
BOOST_FIXTURE_TEST_SUITE(versionbitmap_buffers_test, versionbitmap_buffers_fixture)

BOOST_AUTO_TEST_CASE(buffer_size_test)
{
    BOOST_CHECK_EQUAL(buffer.size(), sizeof(v13_detail::ofp_hello) + (sizeof(v13_detail::ofp_hello_elem_versionbitmap) + bitmaps.size() * sizeof(bitmaps[0]) + 7) / 8 * 8);
}

BOOST_AUTO_TEST_CASE(hello_length_test)
{
    auto length = std::uint16_t{};
    std::memcpy(&length, buffer.data() + offsetof(v13_detail::ofp_header, length), sizeof(length));
    BOOST_CHECK_EQUAL(canard::ntoh(length), buffer.size());
    BOOST_CHECK_EQUAL(canard::ntoh(length) % 8, 0);
}
BOOST_AUTO_TEST_SUITE_END() // versionbitmap_buffers_test

BOOST_AUTO_TEST_SUITE_END() // encode_test

BOOST_AUTO_TEST_CASE(decode_test)
{
    auto const sut = hello{std::vector<std::uint32_t>{3, 4}};
    auto const buffer = sut.encode();
    auto it = buffer.begin();

    auto decoded_message = hello::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
    BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
}

BOOST_AUTO_TEST_SUITE_END() // hello_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

