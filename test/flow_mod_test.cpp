#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE flow_mod_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <canard/network/protocol/openflow/v13/message/flow_mod.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(flow_mod_test)

struct flow_entry_fixture {
    std::array<std::uint8_t, 6> eth_dst = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
    std::array<std::uint8_t, 6> eth_src = {{0x11, 0x12, 0x13, 0x14, 0x15, 0x16}};
    flow_entry entry = {{
        oxm_match{oxm_in_port{4}, oxm_eth_dst{eth_dst}, oxm_eth_src{eth_src}}, OFP_DEFAULT_PRIORITY // 4 + 8 + 10 + 10 = 32
    }, {
          instructions::apply_actions{
            actions::set_field{oxm_eth_dst{eth_src}}, actions::set_field{oxm_eth_src{eth_dst}}, actions::output{4} // 8 + 16 + 16 + 16 = 56
          }
        , instructions::clear_actions{} // 8
        , instructions::write_actions{
            actions::set_field{oxm_eth_dst{eth_src}}, actions::set_field{oxm_eth_src{eth_dst}} // 8 + 16 + 16 = 40
        }
    }};
};

BOOST_FIXTURE_TEST_SUITE(instantiation_test, flow_entry_fixture)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        std::uint8_t const table_id = 255;
        std::uint16_t const flags = OFPFF_SEND_FLOW_REM | OFPFF_CHECK_OVERLAP;

        auto const sut = flow_mod_add{entry, table_id, flags};

        BOOST_CHECK_EQUAL(sut.version(), OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), OFPT_FLOW_MOD);
        BOOST_CHECK_EQUAL(sut.length(), 48 + 32 + 56 + 8 + 40);
        BOOST_CHECK_EQUAL(sut.table_id(), table_id);
        BOOST_CHECK_EQUAL(sut.flags(), flags);
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = flow_mod_add{entry, 0, 0};

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.version(), sut.version());
        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(copy.table_id(), sut.table_id());
        BOOST_CHECK_EQUAL(copy.flags(), sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct flow_mod_fixture : flow_entry_fixture {
    flow_mod_add sut = {entry, 0, OFPFF_SEND_FLOW_REM};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, flow_mod_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        auto buffer = std::vector<std::uint8_t>{};
        sut.encode(buffer);
        auto it = buffer.begin();

        auto const decoded_message = flow_mod_add::decode(it, buffer.end());

        BOOST_CHECK(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_message.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_message.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_message.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_message.xid(), sut.xid());
        BOOST_CHECK_EQUAL(decoded_message.table_id(), sut.table_id());
        BOOST_CHECK_EQUAL(decoded_message.flags(), sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // flow_mod_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
