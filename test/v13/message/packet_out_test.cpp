#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/packet_out.hpp>
#include <boost/test/unit_test.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = canard::network::openflow::v13;
namespace match = v13::oxm_match;
using proto = v13::protocol;

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_SUITE(packet_out_test)

    constexpr auto ofp_pkt_out_size = sizeof(v13::v13_detail::ofp_packet_out);

    BOOST_AUTO_TEST_CASE(constructor_from_buffer_id_test)
    {
        auto const buffer_id = std::uint32_t{0xffffff};
        auto const in_port = 1;
        auto const actions = v13::action_list{
            v13::actions::decrement_nw_ttl{}, v13::actions::output{3}
        };
        auto const xid = std::uint32_t{0xff00ff};

        auto const sut = v13::messages::packet_out{
            buffer_id, in_port, actions, xid
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PACKET_OUT);
        BOOST_TEST(sut.length() == ofp_pkt_out_size + actions.length());
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.in_port() == in_port);
        BOOST_TEST(sut.actions_length() == 24);
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());
    }

    BOOST_AUTO_TEST_CASE(constructor_from_binary_test)
    {
        unsigned char const bin[] = "\x00\x01\x02\x03\x04\x05";
        auto const in_port = proto::OFPP_CONTROLLER;
        auto const actions = v13::action_list{
              v13::actions::push_vlan{0x8100}
            , v13::actions::set_vlan_vid{3}
            , v13::actions::output{3}
        };
        auto const xid = std::uint32_t{0x000001};

        auto const sut = v13::messages::packet_out{
            of::binary_data{bin}, in_port, actions, xid
        };

        BOOST_TEST(sut.length() == ofp_pkt_out_size + actions.length() + sizeof(bin));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.in_port() == in_port);
        BOOST_TEST(sut.actions_length() == 40);
        BOOST_TEST(sut.frame_length() == sizeof(bin));
        BOOST_TEST((sut.frame() == boost::make_iterator_range(bin)));
    }

    BOOST_AUTO_TEST_CASE(constructor_from_empty_actions_test)
    {
        unsigned char const bin[] = "\x10\x11\x12\x13\x14\x15";
        auto const in_port = 2;
        auto const xid = std::uint32_t{0x000001};

        auto const sut = v13::messages::packet_out{
            of::binary_data{bin}, in_port, {}, xid
        };

        BOOST_TEST(sut.length() == ofp_pkt_out_size + sizeof(bin));
        BOOST_TEST(sut.xid() == xid);
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.in_port() == in_port);
        BOOST_TEST(sut.actions_length() == 0);
        BOOST_TEST(sut.frame_length() == sizeof(bin));
        BOOST_TEST((sut.frame() == boost::make_iterator_range(bin)));
    }

    BOOST_AUTO_TEST_CASE(constructor_from_empty_binary_test)
    {
        auto const in_port = 2;

        auto const sut = v13::messages::packet_out{
            of::binary_data{}, in_port, {}
        };

        BOOST_TEST(sut.length() == ofp_pkt_out_size);
        BOOST_TEST(sut.buffer_id() == proto::OFP_NO_BUFFER);
        BOOST_TEST(sut.in_port() == in_port);
        BOOST_TEST(sut.actions_length() == 0);
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_from_empty_binary_test)
    {
        auto const sut = v13::messages::packet_out{
            0, 0, {v13::actions::output::to_controller()}
        };

        auto const copy = sut;

        BOOST_TEST(copy.version() == sut.version());
        BOOST_TEST(copy.type() == sut.type());
        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.in_port() == sut.in_port());
        BOOST_TEST(copy.actions_length() == sut.actions_length());
        BOOST_TEST(copy.frame_length() == sut.frame_length());
        BOOST_TEST((copy.frame() == sut.frame()));
    }

    struct packet_out_fixture
    {
        v13::messages::packet_out sut = v13::messages::packet_out{
              of::binary_data{"\x10\x11\x12\x13\x14\x15"}
            , proto::OFPP_CONTROLLER
            , v13::action_list{
                  v13::actions::push_vlan{0x8100}
                , v13::actions::set_vlan_vid{proto::OFPVID_PRESENT | 3}
                , v13::actions::output{3}
              }
            , 0x1234
        };
    };

    BOOST_FIXTURE_TEST_CASE(copy_constructor_test, packet_out_fixture)
    {
        auto const copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.in_port() == sut.in_port());
        BOOST_TEST(copy.actions_length() == sut.actions_length());
        BOOST_TEST(copy.frame_length() == sut.frame_length());
        BOOST_TEST((copy.frame() == sut.frame()));
    }

    BOOST_FIXTURE_TEST_CASE(move_constructor_test, packet_out_fixture)
    {
        auto const org = sut;

        auto const copy = std::move(sut);

        BOOST_TEST(copy.length() == org.length());
        BOOST_TEST(copy.xid() == org.xid());
        BOOST_TEST(copy.buffer_id() == org.buffer_id());
        BOOST_TEST(copy.in_port() == org.in_port());
        BOOST_TEST(copy.actions_length() == org.actions_length());
        BOOST_TEST(copy.frame_length() == org.frame_length());
        BOOST_TEST((copy.frame() == org.frame()));

        BOOST_TEST(sut.length() == ofp_pkt_out_size);
        BOOST_TEST(sut.actions_length() == 0);
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, packet_out_fixture)
    {
        auto copy = v13::messages::packet_out{0, 0, {}};

        copy = sut;

        BOOST_TEST(copy.length() == sut.length());
        BOOST_TEST(copy.xid() == sut.xid());
        BOOST_TEST(copy.buffer_id() == sut.buffer_id());
        BOOST_TEST(copy.in_port() == sut.in_port());
        BOOST_TEST(copy.actions_length() == sut.actions_length());
        BOOST_TEST(copy.frame_length() == sut.frame_length());
        BOOST_TEST((copy.frame() == sut.frame()));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, packet_out_fixture)
    {
        auto const org = sut;
        auto copy = v13::messages::packet_out{0, 0, {}};

        copy = std::move(sut);

        BOOST_TEST(copy.length() == org.length());
        BOOST_TEST(copy.xid() == org.xid());
        BOOST_TEST(copy.buffer_id() == org.buffer_id());
        BOOST_TEST(copy.in_port() == org.in_port());
        BOOST_TEST(copy.actions_length() == org.actions_length());
        BOOST_TEST(copy.frame_length() == org.frame_length());
        BOOST_TEST((copy.frame() == org.frame()));

        BOOST_TEST(sut.length() == ofp_pkt_out_size);
        BOOST_TEST(sut.actions_length() == 0);
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, packet_out_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());

        auto const expected
            = "\x04\x0d\x00\x47\x00\x00\x12\x34" "\xff\xff\xff\xff\xff\xff\xff\xfd"
              "\x00\x28\x00\x00\x00\x00\x00\x00" "\x00\x11\x00\x08\x81\x00\x00\x00"
              "\x00\x19\x00\x10\x80\x00\x0c\x02" "\x10\x03\x00\x00\x00\x00\x00\x00"
              "\x00\x00\x00\x10\x00\x00\x00\x03" "\xff\xff\x00\x00\x00\x00\x00\x00"
              "\x10\x11\x12\x13\x14\x15\x00"_bin
            ;
        BOOST_TEST(buffer == expected, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, packet_out_fixture)
    {
        char const buffer[]
            = "\x04\x0d\x00\x47\x00\x00\x12\x34" "\xff\xff\xff\xff\xff\xff\xff\xfd"
              "\x00\x28\x00\x00\x00\x00\x00\x00" "\x00\x11\x00\x08\x81\x00\x00\x00"
              "\x00\x19\x00\x10\x80\x00\x0c\x02" "\x10\x03\x00\x00\x00\x00\x00\x00"
              "\x00\x00\x00\x10\x00\x00\x00\x03" "\xff\xff\x00\x00\x00\x00\x00\x00"
              "\x10\x11\x12\x13\x14\x15"
            ;

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer);
        auto pkt_out = v13::messages::packet_out::decode(it, it_end);

        BOOST_TEST(sut.version() == pkt_out.version());
        BOOST_TEST(sut.type() == pkt_out.type());
        BOOST_TEST(sut.length() == pkt_out.length());
        BOOST_TEST(sut.xid() == pkt_out.xid());
        BOOST_TEST(sut.buffer_id() == pkt_out.buffer_id());
        BOOST_TEST(sut.in_port() == pkt_out.in_port());
        BOOST_TEST(sut.actions_length() == pkt_out.actions_length());
        BOOST_TEST(sut.frame_length() == pkt_out.frame_length());
        BOOST_TEST((sut.frame() == pkt_out.frame()));
    }

BOOST_AUTO_TEST_SUITE_END() // packet_out_test

BOOST_AUTO_TEST_SUITE_END() // message_test

