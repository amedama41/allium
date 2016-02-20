#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/packet_queue.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
namespace queue_props = v13::queue_properties;
using proto = v13::protocol;

namespace {

auto operator ""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

struct properties_fixture
{
    std::vector<v13::any_queue_property> properties = {
          v13::any_queue_property{queue_props::min_rate{0x1234}}
        , v13::any_queue_property{queue_props::max_rate{0x5678}}
    }; // 16 + 16 = 32
};

struct packet_queue_fixture : properties_fixture
{
    v13::packet_queue sut{0xf1f2f3f4, 0xf5f6f7f8, properties};
    std::vector<std::uint8_t> binary
        = "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8""\x00\x30\x00\x00\x00\x00\x00\x00"
          "\x00\x01\x00\x10\x00\x00\x00\x00""\x12\x34\x00\x00\x00\x00\x00\x00"
          "\x00\x02\x00\x10\x00\x00\x00\x00""\x56\x78\x00\x00\x00\x00\x00\x00"
          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(packet_queue_test)

    BOOST_FIXTURE_TEST_CASE(construct_from_queue_id_test, properties_fixture)
    {
        auto const queue_id = of::queue_id{32, 43};

        auto const sut = v13::packet_queue{queue_id, properties};

        BOOST_TEST(sut.queue_id() == queue_id.queue());
        BOOST_TEST(sut.port_no() == queue_id.port());
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_packet_queue) + 32);
        BOOST_TEST(sut.properties().size() == properties.size());
        BOOST_TEST(sut.properties()[0].property() == properties[0].property());
        BOOST_TEST(sut.properties()[0].length() == properties[0].length());
        BOOST_TEST(sut.properties()[1].property() == properties[1].property());
        BOOST_TEST(sut.properties()[1].length() == properties[1].length());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, packet_queue_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.properties().size() == sut.properties().size());
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, packet_queue_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.properties().size() == sut.properties().size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_packet_queue));
        BOOST_TEST(src.properties().size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, packet_queue_fixture)
    {
        auto copy = v13::packet_queue{0, 0, {}};
        copy = sut;

        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.properties().size() == sut.properties().size());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, packet_queue_fixture)
    {
        auto copy = v13::packet_queue{0, 0, {}};
        auto src = sut;

        copy = std::move(src);

        BOOST_TEST(copy.queue_id() == sut.queue_id());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.properties().size() == sut.properties().size());
        BOOST_TEST(src.length() == sizeof(v13_detail::ofp_packet_queue));
        BOOST_TEST(src.properties().size() == 0);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, packet_queue_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, packet_queue_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const packet_queue = v13::packet_queue::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(packet_queue.queue_id() == sut.queue_id());
        BOOST_TEST(packet_queue.port_no() == sut.port_no());
        BOOST_TEST(packet_queue.length() == sut.length());
        BOOST_TEST(packet_queue.properties().size() == sut.properties().size());
    }

BOOST_AUTO_TEST_SUITE_END() // packet_queue_test
