#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/table_mod.hpp>
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

BOOST_AUTO_TEST_SUITE(table_mod_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const table_id = proto::OFPTT_MAX;
        auto const config = 0;
        auto const xid = std::uint32_t{0x123};

        auto const sut = v13::messages::table_mod{table_id, config, xid};

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_TABLE_MOD);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_table_mod));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.config() == config);
    }

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<unsigned char>{};
        auto sut = v13::messages::table_mod{
            proto::OFPTT_ALL, proto::OFPTC_DEPRECATED_MASK, 0x234
        };

        sut.encode(buffer);

        char const expected[]
            = "\x04\x11\x00\x10\x00\x00\x02\x34"
              "\xff\x00\x00\x00\x00\x00\x00\x03";
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        char const buffer[]
            = "\x04\x11\x00\x10\x00\x00\x02\x34"
              "\x00\x00\x00\x00\x00\x00\x00\x03";

        auto it = buffer;
        auto it_end = buffer + sizeof(buffer) - 1;
        auto sut = v13::messages::table_mod::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_TABLE_MOD);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_table_mod));
        BOOST_TEST(sut.xid() == 0x234);
        BOOST_TEST(sut.table_id() == 0x0);
        BOOST_TEST(sut.config() == proto::OFPTC_DEPRECATED_MASK);
    }

BOOST_AUTO_TEST_SUITE_END() // table_mod_test

BOOST_AUTO_TEST_SUITE_END() // message_test
