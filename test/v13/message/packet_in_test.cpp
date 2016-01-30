#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/message/packet_in.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <utility>
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

BOOST_AUTO_TEST_SUITE(packet_in_test)

    static constexpr std::size_t data_padding_size = 2;

    BOOST_AUTO_TEST_CASE(constructor_from_binary_test)
    {
        auto const buffer_id = proto::OFP_NO_BUFFER;
        auto const total_len = std::uint16_t{1514};
        auto const reason = proto::OFPR_NO_MATCH;
        auto const table_id = std::uint8_t{0};
        auto const cookie = std::uint64_t{0xffffffffffffffff};
        auto const match = v13::oxm_match{
              v13::match::oxm_in_port{1}
            , v13::match::oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
            , v13::match::oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
            , v13::match::oxm_eth_type{0x0800, 0x0800}
        };
        char const bin[] = "\x00\x01\x02\x03\x04";
        auto data = of::binary_data{bin};

        auto const sut = v13::messages::packet_in{
              buffer_id, total_len, reason, table_id, cookie
            , match, std::move(data)
        };

        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PACKET_IN);
        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_packet_in)
                                 + v13::v13_detail::exact_length(match.length())
                                 + data_padding_size + sizeof(bin));
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.total_length() == total_len);
        BOOST_TEST(sut.reason() == reason);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie);
        // BOOST_TEST(sut.match() == match);
        BOOST_TEST(sut.frame_length() == sizeof(bin));
        BOOST_TEST((sut.frame() == boost::make_iterator_range(bin)));
    }

    BOOST_AUTO_TEST_CASE(constructor_from_range_test)
    {
        auto const buffer_id = 0;
        auto const total_len = std::uint16_t{0};
        auto const reason = proto::OFPR_ACTION;
        auto const table_id = std::uint8_t{0xff};
        auto const cookie = std::uint64_t{0};
        auto const match = v13::oxm_match{};
        char const bin[] = "\x09\x08\x07\x06\x05\x04";

        auto const sut = v13::messages::packet_in{
              buffer_id, total_len, reason, table_id, cookie
            , match, bin
        };

        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_packet_in)
                                 + sizeof(v13::v13_detail::ofp_match)
                                 + data_padding_size + sizeof(bin));
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.total_length() == total_len);
        BOOST_TEST(sut.reason() == reason);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie);
        // BOOST_TEST(sut.match() == match);
        BOOST_TEST(sut.frame_length() == sizeof(bin));
        BOOST_TEST((sut.frame() == boost::make_iterator_range(bin)));
    }

    BOOST_AUTO_TEST_CASE(no_data_contructor)
    {
        auto const buffer_id = 0;
        auto const total_len = std::uint16_t{0};
        auto const reason = proto::OFPR_INVALID_TTL;
        auto const table_id = std::uint8_t{0xff};
        auto const cookie = std::uint64_t{0};
        auto const match = v13::oxm_match{
            v13::match::oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
        };
        auto const bin = std::vector<unsigned char>{};

        auto const sut = v13::messages::packet_in{
              buffer_id, total_len, reason, table_id, cookie
            , match, bin
        };

        BOOST_TEST(sut.length() == sizeof(v13::v13_detail::ofp_packet_in)
                                 + v13::v13_detail::exact_length(match.length())
                                 + data_padding_size);
        BOOST_TEST(sut.buffer_id() == buffer_id);
        BOOST_TEST(sut.total_length() == total_len);
        BOOST_TEST(sut.reason() == reason);
        BOOST_TEST(sut.table_id() == table_id);
        BOOST_TEST(sut.cookie() == cookie);
        // BOOST_TEST(sut.match() == match);
        BOOST_TEST(sut.frame_length() == bin.size());
        BOOST_TEST((sut.frame() == boost::make_iterator_range(bin)));
    }

    struct packet_in_fixture
    {
        v13::messages::packet_in pkt_in = v13::messages::packet_in{
              0x01234567, 512, proto::OFPR_NO_MATCH, 2, 0x0001020304050607
            , v13::oxm_match{
                  v13::match::oxm_in_port{1}
                , v13::match::oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
                , v13::match::oxm_eth_src{{{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}}}
                , v13::match::oxm_eth_type{0x0800, 0x0800}
              }
            , "\x00\xff\x01\xff\x02\xff\x03\xff\x04\xff\x05\xff\x06\xff\x07\xff"
            , 0x00010203
        };
    };

    BOOST_FIXTURE_TEST_CASE(copy_constructor_test, packet_in_fixture)
    {
        auto const sut = pkt_in;

        BOOST_TEST(sut.length() == pkt_in.length());
        BOOST_TEST(sut.xid() == pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == pkt_in.total_length());
        BOOST_TEST(sut.reason() == pkt_in.reason());
        BOOST_TEST(sut.table_id() == pkt_in.table_id());
        BOOST_TEST(sut.cookie() == pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == pkt_in.frame_length());
        BOOST_TEST((sut.frame() == pkt_in.frame()));
    }

    BOOST_FIXTURE_TEST_CASE(move_constructor_test, packet_in_fixture)
    {
        auto org_pkt_in = pkt_in;

        auto const sut = std::move(pkt_in);

        BOOST_TEST(sut.length() == org_pkt_in.length());
        BOOST_TEST(sut.xid() == org_pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == org_pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == org_pkt_in.total_length());
        BOOST_TEST(sut.reason() == org_pkt_in.reason());
        BOOST_TEST(sut.table_id() == org_pkt_in.table_id());
        BOOST_TEST(sut.cookie() == org_pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == org_pkt_in.frame_length());
        BOOST_TEST((sut.frame() == org_pkt_in.frame()));

        BOOST_TEST(pkt_in.length() == sizeof(v13::v13_detail::ofp_packet_in)
                                    + sizeof(v13::v13_detail::ofp_match)
                                    + data_padding_size);
        BOOST_TEST(pkt_in.frame().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, packet_in_fixture)
    {
        auto sut = v13::messages::packet_in{
            0, 0, proto::OFPR_ACTION, 0, 0, {}, ""
        };

        sut = pkt_in;

        BOOST_TEST(sut.length() == pkt_in.length());
        BOOST_TEST(sut.xid() == pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == pkt_in.total_length());
        BOOST_TEST(sut.reason() == pkt_in.reason());
        BOOST_TEST(sut.table_id() == pkt_in.table_id());
        BOOST_TEST(sut.cookie() == pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == pkt_in.frame_length());
        BOOST_TEST((sut.frame() == pkt_in.frame()));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, packet_in_fixture)
    {
        auto org_pkt_in = pkt_in;
        auto sut = v13::messages::packet_in{
            0, 0, proto::OFPR_INVALID_TTL, 0, 0, {}, ""
        };

        sut = std::move(pkt_in);

        BOOST_TEST(sut.length() == org_pkt_in.length());
        BOOST_TEST(sut.xid() == org_pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == org_pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == org_pkt_in.total_length());
        BOOST_TEST(sut.reason() == org_pkt_in.reason());
        BOOST_TEST(sut.table_id() == org_pkt_in.table_id());
        BOOST_TEST(sut.cookie() == org_pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == org_pkt_in.frame_length());
        BOOST_TEST((sut.frame() == org_pkt_in.frame()));

        BOOST_TEST(pkt_in.length() == sizeof(v13::v13_detail::ofp_packet_in)
                                    + sizeof(v13::v13_detail::ofp_match)
                                    + data_padding_size);
        BOOST_TEST(pkt_in.frame().empty());
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, packet_in_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        pkt_in.encode(buffer);

        BOOST_TEST(buffer.size() == pkt_in.length());
        char const expected[]
            = "\x04\x0a\x00\x53\x00\x01\x02\x03" "\x01\x23\x45\x67\x02\x00\x00\x02"
              "\x00\x01\x02\x03\x04\x05\x06\x07" "\x00\x01\x00\x28\x80\x00\x00\x04"
              "\x00\x00\x00\x01\x80\x00\x06\x06" "\x01\x02\x03\x04\x05\x06\x80\x00"
              "\x08\x06\x11\x12\x13\x14\x15\x16" "\x80\x00\x0b\x04\x08\x00\x08\x00"
              "\x00\x00\x00\xff\x01\xff\x02\xff" "\x03\xff\x04\xff\x05\xff\x06\xff"
              "\x07\xff\x00"
            ;
        BOOST_TEST(buffer == to_buffer(expected), boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, packet_in_fixture)
    {
        char const buffer[]
            = "\x04\x0a\x00\x53\x00\x01\x02\x03" "\x01\x23\x45\x67\x02\x00\x00\x02"
              "\x00\x01\x02\x03\x04\x05\x06\x07" "\x00\x01\x00\x28\x80\x00\x00\x04"
              "\x00\x00\x00\x01\x80\x00\x06\x06" "\x01\x02\x03\x04\x05\x06\x80\x00"
              "\x08\x06\x11\x12\x13\x14\x15\x16" "\x80\x00\x0b\x04\x08\x00\x08\x00"
              "\x00\x00\x00\xff\x01\xff\x02\xff" "\x03\xff\x04\xff\x05\xff\x06\xff"
              "\x07\xff\x00"
            ;

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer) - 1;
        auto const sut = v13::messages::packet_in::decode(it, it_end);

        BOOST_TEST(sut.version() == pkt_in.version());
        BOOST_TEST(sut.type() == pkt_in.type());
        BOOST_TEST(sut.length() == pkt_in.length());
        BOOST_TEST(sut.xid() == pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == pkt_in.total_length());
        BOOST_TEST(sut.reason() == pkt_in.reason());
        BOOST_TEST(sut.table_id() == pkt_in.table_id());
        BOOST_TEST(sut.cookie() == pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == pkt_in.frame_length());
        BOOST_TEST((sut.frame() == pkt_in.frame()));
    }

    BOOST_AUTO_TEST_CASE(decode_no_data_test)
    {
        char const buffer[]
            = "\x04\x0a\x00\x2a\x00\x01\x02\x03" "\x01\x23\x45\x67\x02\x00\x00\x02"
              "\x00\x01\x02\x03\x04\x05\x06\x07" "\x00\x01\x00\x0c\x80\x00\x00\x04"
              "\x00\x00\x00\x01\x00\x00\x00\x00" "\x00\x00"
            ;

        auto it = buffer;
        auto const it_end = buffer + sizeof(buffer) - 1;
        auto const sut = v13::messages::packet_in::decode(it, it_end);

        BOOST_TEST(it == it_end);
        BOOST_TEST(sut.version() == proto::OFP_VERSION);
        BOOST_TEST(sut.type() == proto::OFPT_PACKET_IN);
        BOOST_TEST(sut.length() == sizeof(buffer) - 1);
        BOOST_TEST(sut.xid() == 0x00010203);
        BOOST_TEST(sut.buffer_id() == 0x1234567);
        BOOST_TEST(sut.total_length() == 0x200);
        BOOST_TEST(sut.reason() == 0);
        BOOST_TEST(sut.table_id() == 2);
        BOOST_TEST(sut.cookie() == 0x0001020304050607);
        BOOST_TEST(sut.match().length() == 12);
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());
    }

    BOOST_FIXTURE_TEST_CASE(extract_frame_test, packet_in_fixture)
    {
        auto sut = pkt_in;

        auto binary = sut.extract_frame();

        BOOST_TEST(sut.length() + binary.size() == pkt_in.length());
        BOOST_TEST(sut.xid() == pkt_in.xid());
        BOOST_TEST(sut.buffer_id() == pkt_in.buffer_id());
        BOOST_TEST(sut.total_length() == pkt_in.total_length());
        BOOST_TEST(sut.reason() == pkt_in.reason());
        BOOST_TEST(sut.table_id() == pkt_in.table_id());
        BOOST_TEST(sut.cookie() == pkt_in.cookie());
        BOOST_TEST(sut.frame_length() == 0);
        BOOST_TEST(sut.frame().empty());

        auto const binary_range
            = boost::make_iterator_range(binary.begin(), binary.end());
        BOOST_TEST((binary_range == pkt_in.frame()));
    }

BOOST_AUTO_TEST_SUITE_END() // packet_in_test

BOOST_AUTO_TEST_SUITE_END() // message_test

