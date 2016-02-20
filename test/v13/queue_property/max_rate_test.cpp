#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/queue_property/max_rate.hpp>
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

struct max_rate_fixture
{
    queue_props::max_rate sut{0xf1f2};
    std::vector<std::uint8_t> binary
        = "\x00\x02\x00\x10\x00\x00\x00\x00""\xf1\xf2\x00\x00\x00\x00\x00\x00"
          ""_bin
        ;
};

}

BOOST_AUTO_TEST_SUITE(queue_property_test)

BOOST_AUTO_TEST_SUITE(max_rate_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const rate = std::uint16_t{1};

        auto const sut = queue_props::max_rate{rate};

        BOOST_TEST(sut.property() == proto::OFPQT_MAX_RATE);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_queue_prop_max_rate));
        BOOST_TEST(sut.rate() == rate);
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, max_rate_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, max_rate_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const max_rate = queue_props::max_rate::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(max_rate.property() == sut.property());
        BOOST_TEST(max_rate.length() == sut.length());
        BOOST_TEST(max_rate.rate() == sut.rate());
    }

BOOST_AUTO_TEST_SUITE_END() // max_rate_test

BOOST_AUTO_TEST_SUITE_END() // queue_property_test
