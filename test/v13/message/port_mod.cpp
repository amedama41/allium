#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/port_mod.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace v13_detail = v13::v13_detail;
using proto = v13::protocol;

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(port_mod_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const port_no = std::uint32_t{proto::OFPP_MAX};
        auto const hw_addr
            = canard::mac_address{{0x10, 0x20, 0x30, 0x40, 0x50, 0x60}};
        auto const config = std::uint32_t{proto::OFPPC_NO_RECV};
        auto const mask = std::uint32_t{
            proto::OFPPC_PORT_DOWN | proto::OFPPC_NO_RECV
        };
        auto const advertise = std::uint32_t{
            proto::OFPPF_10GB_FD | proto::OFPPF_AUTONEG
        };
        auto const xid = std::uint32_t{0x01010101};

        auto const sut = v13::messages::port_mod{
            port_no, hw_addr, config, mask, advertise, xid
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PORT_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_port_mod));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.hardware_address() == hw_addr);
        BOOST_TEST(sut.config() == config);
        BOOST_TEST(sut.mask() == mask);
        BOOST_TEST(sut.advertised_features() == advertise);
    }

    BOOST_AUTO_TEST_CASE(construct_from_port_test)
    {
        auto const port = v13::port::from_ofp_port({
              9, {0}, {0x11, 0x21, 0x31, 0x41, 0x51, 0x61}, {0}
            , "eth0", proto::OFPPC_PORT_DOWN, proto::OFPPS_LINK_DOWN
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER
            , proto::OFPPF_10GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_FIBER | proto::OFPPF_AUTONEG
            , proto::OFPPF_10GB_FD | proto::OFPPF_1GB_FD | proto::OFPPF_COPPER | proto::OFPPF_AUTONEG
            , 10000, 12000
        });
        auto const config = std::uint32_t{proto::OFPPC_NO_RECV};
        auto const mask = std::uint32_t{
            proto::OFPPC_PORT_DOWN | proto::OFPPC_NO_RECV
        };
        auto const advertise = std::uint32_t{
            proto::OFPPF_10GB_FD | proto::OFPPF_AUTONEG
        };
        auto const xid = std::uint32_t{0x01010101};

        auto const sut = v13::messages::port_mod{
            port, config, mask, advertise, xid
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PORT_MOD);
        BOOST_TEST(sut.length() == sizeof(v13_detail::ofp_port_mod));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.port_no() == port.port_no());
        BOOST_TEST(sut.hardware_address() == port.hardware_address());
        BOOST_TEST(sut.config() == config);
        BOOST_TEST(sut.mask() == mask);
        BOOST_TEST(sut.advertised_features() == advertise);
    }

    struct port_mod_fixture
    {
        v13::messages::port_mod const sut{
              proto::OFPP_MAX
            , canard::mac_address{{0xff, 0x11, 0xff, 0x12, 0x13, 0x14}}
            , proto::OFPPC_NO_FWD
            , proto::OFPPC_PORT_DOWN | proto::OFPPC_NO_RECV
            , proto::OFPPF_10GB_FD | proto::OFPPF_AUTONEG
            , 0x01010101
        };
        std::vector<std::uint8_t> bin_port_mod
            = "\x04\x10\x00\x28\x01\x01\x01\x01" "\xff\xff\xff\x00\x00\x00\x00\x00"
              "\xff\x11\xff\x12\x13\x14\x00\x00" "\x00\x00\x00\x20\x00\x00\x00\x05"
              "\x00\x00\x20\x40\x00\x00\x00\x00"_bin
            ;
    };

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, port_mod_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.port_no() == sut.port_no());
        BOOST_TEST(copy.hardware_address() == sut.hardware_address());
        BOOST_TEST(copy.config() == sut.config());
        BOOST_TEST(copy.mask() == sut.mask());
        BOOST_TEST(copy.advertised_features() == sut.advertised_features());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, port_mod_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == bin_port_mod, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, port_mod_fixture)
    {
        auto it = bin_port_mod.begin();
        auto const it_end = bin_port_mod.end();

        auto const port_mod = v13::messages::port_mod::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST(port_mod.version() == sut.version());
        BOOST_TEST(port_mod.type() == sut.type());
        BOOST_TEST(port_mod.length() == sut.length());
        BOOST_TEST(port_mod.xid() == sut.xid());
        BOOST_TEST(port_mod.port_no() == sut.port_no());
        BOOST_TEST(port_mod.hardware_address() == sut.hardware_address());
        BOOST_TEST(port_mod.config() == sut.config());
        BOOST_TEST(port_mod.mask() == sut.mask());
        BOOST_TEST(port_mod.advertised_features() == sut.advertised_features());
    }

BOOST_AUTO_TEST_SUITE_END() // port_mod_test

BOOST_AUTO_TEST_SUITE_END() // message_test

