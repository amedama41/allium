#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/port_status.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

inline auto operator ""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(port_status_test)

    struct ofp_port_fixture
    {
        v13_detail::ofp_port const ofp_port{
              1, {0}, {0x10, 0x20, 0x30, 0x40, 0x50, 0x60}, {0}
            , "eth0", proto::OFPPC_PORT_DOWN, proto::OFPPS_LINK_DOWN
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_COPPER | proto::OFPPF_AUTONEG
            , 10000, 12000
        };
    };

    BOOST_FIXTURE_TEST_CASE(construct_test, ofp_port_fixture)
    {
        auto const reason = proto::OFPPR_ADD;
        auto const xid = 0x0123;

        auto const sut = v13::messages::port_status{
            reason, v13::port::from_ofp_port(ofp_port), xid
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PORT_STATUS);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_port_status));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.reason() == reason);
        BOOST_TEST(sut.port_no() == ofp_port.port_no);
        BOOST_TEST(sut.hardware_address() == canard::mac_address{ofp_port.hw_addr});
        BOOST_TEST(sut.name() == ofp_port.name);
        BOOST_TEST(sut.config() == ofp_port.config);
        BOOST_TEST(sut.state() == ofp_port.state);
        BOOST_TEST(sut.current_features() == ofp_port.curr);
        BOOST_TEST(sut.advertised_features() == ofp_port.advertised);
        BOOST_TEST(sut.supported_features() == ofp_port.supported);
        BOOST_TEST(sut.peer_advertised_features() == ofp_port.peer);
        BOOST_TEST(sut.current_speed() == ofp_port.curr_speed);
        BOOST_TEST(sut.max_speed() == ofp_port.max_speed);
    }

    struct port_status_fixture {
        v13_detail::ofp_port const ofp_port{
              proto::OFPP_MAX, {0}, {0x10, 0x20, 0x30, 0x40, 0x50, 0x60}, {0}
            , "eth1"
            , proto::OFPPC_PORT_DOWN | proto::OFPPC_NO_FWD
            , proto::OFPPS_LINK_DOWN | proto::OFPPS_LIVE
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_COPPER | proto::OFPPF_AUTONEG
            , 10000, 12000
        };
        v13::messages::port_status const sut{
            proto::OFPPR_DELETE, v13::port::from_ofp_port(ofp_port), 0x87654321
        };
        std::vector<std::uint8_t> bin_port_status
            = "\x04\x0c\x00\x50\x87\x65\x43\x21" "\x01\x00\x00\x00\x00\x00\x00\x00"
              "\xff\xff\xff\x00\x00\x00\x00\x00" "\x10\x20\x30\x40\x50\x60\x00\x00"
              "e" "t" "h" "1" "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00"
              "\x00\x00\x00\x21\x00\x00\x00\x05" "\x00\x00\x10\x40\x00\x00\x30\x40"
              "\x00\x00\x30\x60\x00\x00\x28\x60" "\x00\x00\x27\x10\x00\x00\x2e\xe0"_bin
            ;
    };

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_status_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.reason() == sut.reason());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.hardware_address() == sut.hardware_address());
        BOOST_TEST(copy.name() == sut.name());
        BOOST_TEST(copy.config() == sut.config());
        BOOST_TEST(copy.state() == sut.state());
        BOOST_TEST(copy.current_features() == sut.current_features());
        BOOST_TEST(copy.advertised_features() == sut.advertised_features());
        BOOST_TEST(copy.supported_features() == sut.supported_features());
        BOOST_TEST(copy.peer_advertised_features() == sut.peer_advertised_features());
        BOOST_TEST(copy.current_speed() == sut.current_speed());
        BOOST_TEST(copy.max_speed() == sut.max_speed());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_status_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_port_status, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_status_fixture)
    {
        auto it = bin_port_status.begin();
        auto const it_end = bin_port_status.end();

        auto const port_status = v13::messages::port_status::decode(it, it_end);

        BOOST_TEST((it == it_end));

        BOOST_TEST(port_status.version() == sut.version());
        BOOST_TEST(port_status.type() == sut.type());
        BOOST_TEST(port_status.length() == sut.length());
        BOOST_TEST(port_status.xid() == sut.xid());
        BOOST_TEST(port_status.reason() == sut.reason());
        BOOST_TEST(port_status.port_no() == sut.port_no());
        BOOST_TEST(port_status.hardware_address() == sut.hardware_address());
        BOOST_TEST(port_status.name() == sut.name());
        BOOST_TEST(port_status.config() == sut.config());
        BOOST_TEST(port_status.state() == sut.state());
        BOOST_TEST(port_status.current_features() == sut.current_features());
        BOOST_TEST(port_status.advertised_features() == sut.advertised_features());
        BOOST_TEST(port_status.supported_features() == sut.supported_features());
        BOOST_TEST(port_status.peer_advertised_features() == sut.peer_advertised_features());
        BOOST_TEST(port_status.current_speed() == sut.current_speed());
        BOOST_TEST(port_status.max_speed() == sut.max_speed());
    }

BOOST_AUTO_TEST_SUITE_END() // port_status_test

BOOST_AUTO_TEST_SUITE_END() // message_test
