#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v10/action/set_field.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <canard/mac_address.hpp>

namespace of = canard::network::openflow;
namespace v10 = of::v10;
namespace detail = v10::v10_detail;
namespace actions = v10::actions;

namespace {

auto operator""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

struct set_eth_src_fixture
{
    actions::set_eth_src sut{
        canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
    };
    std::vector<std::uint8_t> binary
        = "\x00\x04\x00\x10\x01\x02\x03\x04""\x05\x06\x00\x00\x00\x00\x00\x00"
          ""_bin;
};

struct set_ipv4_dst_fixture
{
    actions::set_ipv4_dst sut{
        boost::asio::ip::address_v4::from_string("192.168.1.254")
    };
    std::vector<std::uint8_t> binary
        = "\x00\x07\x00\x08\xc0\xa8\x01\xfe"_bin;
};

struct set_tcp_src_fixture
{
    actions::set_tcp_src sut{0x1234};
    std::vector<std::uint8_t> binary
        = "\x00\x09\x00\x08\x12\x34\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(actions_test)

BOOST_AUTO_TEST_SUITE(set_eth_src_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const mac
            = canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};

        auto const sut = actions::set_eth_src{mac};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_SET_DL_SRC);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_dl_addr));
        BOOST_TEST(sut.value() == mac);
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const mac
            = canard::mac_address{{0x10, 0x20, 0x30, 0x40, 0x50, 0x60}};

        auto const sut = actions::set_eth_src::create(mac);

        BOOST_TEST(sut.value() == mac);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = actions::set_eth_src{
            canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        auto const sut2 = actions::set_eth_src{
            canard::mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}
        };
        auto const sut3 = actions::set_eth_src{
            canard::mac_address{{0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6}}
        };

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_eth_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_eth_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_eth_src = actions::set_eth_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_eth_src == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_eth_src_test

BOOST_AUTO_TEST_SUITE(set_ipv4_dst_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const addr
            = boost::asio::ip::address_v4::from_string("127.0.0.1");

        auto const sut = actions::set_ipv4_dst{addr};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_SET_NW_DST);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_nw_addr));
        BOOST_TEST(sut.value() == addr);
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const addr
            = boost::asio::ip::address_v4::from_string("255.255.255.0");

        auto const sut = actions::set_ipv4_dst::create(addr);

        BOOST_TEST(sut.value() == addr);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        using boost::asio::ip::address_v4;
        auto const sut1 = actions::set_ipv4_dst{
            address_v4::from_string("127.0.0.1")
        };
        auto const sut2 = actions::set_ipv4_dst{
            address_v4::from_string("127.0.0.1")
        };
        auto const sut3 = actions::set_ipv4_dst{
            address_v4::from_string("192.168.1.255")
        };

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_ipv4_dst_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_ipv4_dst_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_ipv4_dst = actions::set_ipv4_dst::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_ipv4_dst == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_ipv4_dst_test

BOOST_AUTO_TEST_SUITE(set_tcp_src_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port = std::uint16_t{53};

        auto const sut = actions::set_tcp_src{port};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_SET_TP_SRC);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_tp_port));
        BOOST_TEST(sut.value() == port);
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const port = 6653;

        auto const sut = actions::set_tcp_src::create(port);

        BOOST_TEST(sut.value() == port);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = actions::set_tcp_src{0x1234};
        auto const sut2 = actions::set_tcp_src{0x1234};
        auto const sut3 = actions::set_tcp_src{0x4321};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, set_tcp_src_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, set_tcp_src_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const set_tcp_src = actions::set_tcp_src::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((set_tcp_src == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // set_tcp_src_test

BOOST_AUTO_TEST_SUITE_END() // actions_test
