#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <boost/endian/conversion.hpp>
#include <canard/unit_test.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>

using boost::asio::ip::address_v4;
using canard::mac_address;

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(instruction_set_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = instruction_set{};

        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

    BOOST_AUTO_TEST_CASE(variadic_template_constructor_test)
    {
        auto lvalue = instructions::write_actions{
            actions::set_ipv4_src{address_v4{0x7f000001}}, actions::set_ipv4_dst{address_v4{0x7f000002}}
        }; // 8 + 16 + 16 = 40
        auto const clvalue = instructions::clear_actions{}; // 8
        auto rvalue = instructions::apply_actions{
            actions::set_eth_type{0x0800}, actions::set_eth_src{mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}}
        }; // 8 + 16 + 16 = 40
        auto const sut = instruction_set{
              instructions::write_actions{
                    actions::set_queue{4}
                  , actions::set_ipv4_src{address_v4{0x7f000003}}
              } // 8 + 8 + 16 = 32
            , instructions::goto_table{4} // 8
            , lvalue, clvalue, std::move(rvalue)
        };

        BOOST_CHECK_EQUAL(sut.length(), 96);
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_test)
    {
        auto sut = instruction_set{
              instructions::write_actions{ actions::set_queue{4}, actions::set_ip_dscp{4}, actions::output{4} }
            , instructions::goto_table{2}
        };

        auto const copy = sut;

        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = instruction_set{
              instructions::write_actions{ actions::set_queue{4}, actions::set_ip_dscp{4}, actions::output{4} }
            , instructions::goto_table{2}
        };

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.length(), 56);
        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test


BOOST_AUTO_TEST_SUITE(assignment_test)

    BOOST_AUTO_TEST_CASE(copy_assign_test)
    {
        auto sut = instruction_set{
              instructions::write_actions{ actions::set_queue{4}, actions::set_ip_dscp{4}, actions::output{4} }
            , instructions::goto_table{2}
        };
        auto copy = instruction_set{};

        copy = sut;

        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_AUTO_TEST_CASE(move_assign_test)
    {
        auto sut = instruction_set{
              instructions::write_actions{ actions::set_queue{4}, actions::set_ip_dscp{4}, actions::output{4} }
            , instructions::goto_table{2}
        };
        auto copy = instruction_set{};

        copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.length(), 56);
        BOOST_CHECK_EQUAL(sut.length(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // assignment_test

struct empty_instruction_set
{
    instruction_set sut{};
};
BOOST_FIXTURE_TEST_SUITE(add_test, empty_instruction_set)

    BOOST_AUTO_TEST_CASE(add_prvalue_instructions)
    {
        sut.add(instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}});
        sut.add(instructions::clear_actions{});
        sut.add(instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}});

        BOOST_CHECK_EQUAL(sut.length(), 88);
    }

    BOOST_AUTO_TEST_CASE(add_lvalue_instructions)
    {
        auto apply_lvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto clear_lvalue = instructions::clear_actions{};
        auto write_lvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(apply_lvalue);
        sut.add(clear_lvalue);
        sut.add(write_lvalue);

        BOOST_CHECK_EQUAL(sut.length(), 88);
    }

    BOOST_AUTO_TEST_CASE(add_const_lvalue_instructions)
    {
        auto const apply_clvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto const clear_clvalue = instructions::clear_actions{};
        auto const write_clvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(apply_clvalue);
        sut.add(clear_clvalue);
        sut.add(write_clvalue);

        BOOST_CHECK_EQUAL(sut.length(), 88);
    }

    BOOST_AUTO_TEST_CASE(add_rvalue_instructions)
    {
        auto apply_rvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto clear_rvalue = instructions::clear_actions{};
        auto write_rvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(std::move(apply_rvalue));
        sut.add(std::move(clear_rvalue));
        sut.add(std::move(write_rvalue));

        BOOST_CHECK_EQUAL(sut.length(), 88);
    }

BOOST_AUTO_TEST_SUITE_END() // add_test

struct write_clear_apply_goto_instruction_set
{
    instruction_set sut{
          instructions::write_actions{ actions::set_queue{4}, actions::set_ipv4_src{address_v4{0x7f000003}} }
        , instructions::clear_actions{}
        , instructions::apply_actions{ actions::set_eth_type{0x0800}, actions::set_eth_src{mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}} }
        , instructions::goto_table{4}
    };
};
BOOST_FIXTURE_TEST_SUITE(update_test, write_clear_apply_goto_instruction_set)

    BOOST_AUTO_TEST_CASE(add_prvalue_instructions)
    {
        sut.add(instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}});
        sut.add(instructions::clear_actions{});
        sut.add(instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}});

        BOOST_CHECK_EQUAL(sut.length(), 96);
    }

    BOOST_AUTO_TEST_CASE(add_lvalue_instructions)
    {
        auto apply_lvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto clear_lvalue = instructions::clear_actions{};
        auto write_lvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(apply_lvalue);
        sut.add(clear_lvalue);
        sut.add(write_lvalue);

        BOOST_CHECK_EQUAL(sut.length(), 96);
    }

    BOOST_AUTO_TEST_CASE(add_const_lvalue_instructions)
    {
        auto const apply_clvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto const clear_clvalue = instructions::clear_actions{};
        auto const write_clvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(apply_clvalue);
        sut.add(clear_clvalue);
        sut.add(write_clvalue);

        BOOST_CHECK_EQUAL(sut.length(), 96);
    }

    BOOST_AUTO_TEST_CASE(add_rvalue_instructions)
    {
        auto apply_rvalue = instructions::apply_actions{actions::set_ipv4_src{address_v4{0x7f000001}}, actions::output{4}};
        auto clear_rvalue = instructions::clear_actions{};
        auto write_rvalue = instructions::write_actions{actions::set_ipv4_dst{address_v4{0x7f000002}}, actions::output{6}};

        sut.add(std::move(apply_rvalue));
        sut.add(std::move(clear_rvalue));
        sut.add(std::move(write_rvalue));

        BOOST_CHECK_EQUAL(sut.length(), 96);
    }

BOOST_AUTO_TEST_SUITE_END() // update_test

BOOST_AUTO_TEST_CASE(encode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instruction_set{
          instructions::write_actions{ actions::set_queue{4}, actions::set_ipv4_src{address_v4{0x7f000003}} }
        , instructions::clear_actions{}
        , instructions::apply_actions{ actions::set_eth_type{0x0800}, actions::set_eth_src{mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}} }
        , instructions::goto_table{4}
    };

    sut.encode(buffer);

    auto index = std::size_t{0};

    {
        auto header = v13_detail::ofp_instruction_actions{};
        auto expected_header = v13_detail::ofp_instruction_actions{
            protocol::OFPIT_APPLY_ACTIONS, 40, {0, 0, 0, 0}
        };
        std::memcpy(&header, &buffer[index], sizeof(header));
        header = boost::endian::big_to_native(header);
        index += sizeof(header);
        BOOST_CHECK_EQUAL(header.type, expected_header.type);
        BOOST_CHECK_EQUAL(header.len, expected_header.len);
        CANARD_CHECK_EQUAL_COLLECTIONS(header.pad, expected_header.pad);

        {
            auto set_field_header = v13_detail::ofp_action_set_field{};
            auto expected_set_field_header = v13_detail::ofp_action_set_field{
                protocol::OFPAT_SET_FIELD, 16, {0}
            };
            std::memcpy(&set_field_header, &buffer[index], sizeof(set_field_header));
            set_field_header = boost::endian::big_to_native(set_field_header);
            index += offsetof(v13_detail::ofp_action_set_field, field);
            BOOST_CHECK_EQUAL(set_field_header.type, expected_set_field_header.type);
            BOOST_CHECK_EQUAL(set_field_header.len, expected_set_field_header.len);

            auto oxm_header = std::uint32_t{};
            auto expected_oxm_header = (oxm_match::eth_type::oxm_type() << 9) | 2U;
            std::memcpy(&oxm_header, &buffer[index], sizeof(oxm_header));
            oxm_header = boost::endian::big_to_native(oxm_header);
            index += sizeof(oxm_header);
            BOOST_CHECK_EQUAL(oxm_header, expected_oxm_header);

            auto oxm_value = std::uint16_t{};
            auto expected_oxm_value = std::uint16_t{0x0800};
            std::memcpy(&oxm_value, &buffer[index], sizeof(oxm_value));
            oxm_value = boost::endian::big_to_native(oxm_value);
            index += sizeof(oxm_value);
            BOOST_CHECK_EQUAL(oxm_value, expected_oxm_value);

            auto expected_pad_value = std::array<std::uint8_t, 6>{{0, 0, 0, 0, 0, 0}};
            BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[index], &buffer[index] + expected_pad_value.size(), expected_pad_value.begin(), expected_pad_value.end());
            index += expected_pad_value.size();
        }
        {
            auto set_field_header = v13_detail::ofp_action_set_field{};
            auto expected_set_field_header = v13_detail::ofp_action_set_field{
                protocol::OFPAT_SET_FIELD, 16, {0}
            };
            std::memcpy(&set_field_header, &buffer[index], sizeof(set_field_header));
            set_field_header = boost::endian::big_to_native(set_field_header);
            index += offsetof(v13_detail::ofp_action_set_field, field);
            BOOST_CHECK_EQUAL(set_field_header.type, expected_set_field_header.type);
            BOOST_CHECK_EQUAL(set_field_header.len, expected_set_field_header.len);

            auto oxm_header = std::uint32_t{};
            auto expected_oxm_header = (oxm_match::eth_src::oxm_type() << 9) | 6U;
            std::memcpy(&oxm_header, &buffer[index], sizeof(oxm_header));
            oxm_header = boost::endian::big_to_native(oxm_header);
            index += sizeof(oxm_header);
            BOOST_CHECK_EQUAL(oxm_header, expected_oxm_header);

            auto oxm_value = std::array<std::uint8_t, 6>{};
            auto expected_oxm_value = std::array<std::uint8_t, 6>{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
            std::memcpy(&oxm_value[0], &buffer[index], oxm_value.size());
            index += oxm_value.size();
            CANARD_CHECK_EQUAL_COLLECTIONS(oxm_value, expected_oxm_value);

            auto expected_pad_value = std::array<std::uint8_t, 2>{{0}};
            BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[index], &buffer[index] + expected_pad_value.size(), expected_pad_value.begin(), expected_pad_value.end());
            index += expected_pad_value.size();
        }
    }
    {
        auto header = v13_detail::ofp_instruction_actions{};
        auto expected_header = v13_detail::ofp_instruction_actions{
            protocol::OFPIT_CLEAR_ACTIONS, 8, {0, 0, 0, 0}
        };
        std::memcpy(&header, &buffer[index], sizeof(header));
        header = boost::endian::big_to_native(header);
        index += sizeof(header);
        BOOST_CHECK_EQUAL(header.type, expected_header.type);
        BOOST_CHECK_EQUAL(header.len, expected_header.len);
        CANARD_CHECK_EQUAL_COLLECTIONS(header.pad, expected_header.pad);
    }
    {
        auto header = v13_detail::ofp_instruction_actions{};
        auto expected_header = v13_detail::ofp_instruction_actions{
            protocol::OFPIT_WRITE_ACTIONS, 32, {0, 0, 0, 0}
        };
        std::memcpy(&header, &buffer[index], sizeof(header));
        header = boost::endian::big_to_native(header);
        index += sizeof(header);
        BOOST_CHECK_EQUAL(header.type, expected_header.type);
        BOOST_CHECK_EQUAL(header.len, expected_header.len);
        CANARD_CHECK_EQUAL_COLLECTIONS(header.pad, expected_header.pad);

        {
            auto set_field_header = v13_detail::ofp_action_set_field{};
            auto expected_set_field_header = v13_detail::ofp_action_set_field{
                protocol::OFPAT_SET_FIELD, 16, {0}
            };
            std::memcpy(&set_field_header, &buffer[index], sizeof(set_field_header));
            set_field_header = boost::endian::big_to_native(set_field_header);
            index += offsetof(v13_detail::ofp_action_set_field, field);
            BOOST_CHECK_EQUAL(set_field_header.type, expected_set_field_header.type);
            BOOST_CHECK_EQUAL(set_field_header.len, expected_set_field_header.len);

            auto oxm_header = std::uint32_t{};
            auto expected_oxm_header = (oxm_match::ipv4_src::oxm_type() << 9) | 4U;
            std::memcpy(&oxm_header, &buffer[index], sizeof(oxm_header));
            oxm_header = boost::endian::big_to_native(oxm_header);
            index += sizeof(oxm_header);
            BOOST_CHECK_EQUAL(oxm_header, expected_oxm_header);

            auto oxm_value = std::uint32_t{};
            auto expected_oxm_value = std::uint32_t{0x7f000003};
            std::memcpy(&oxm_value, &buffer[index], sizeof(oxm_value));
            oxm_value = boost::endian::big_to_native(oxm_value);
            index += sizeof(oxm_value);
            BOOST_CHECK_EQUAL(oxm_value, expected_oxm_value);

            auto expected_pad_value = std::array<std::uint8_t, 4>{{0, 0, 0, 0}};
            BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[index], &buffer[index] + expected_pad_value.size(), expected_pad_value.begin(), expected_pad_value.end());
            index += expected_pad_value.size();
        }
        {
            auto set_queue_header = v13_detail::ofp_action_set_queue{};
            auto expected_set_queue_header = v13_detail::ofp_action_set_queue{
                protocol::OFPAT_SET_QUEUE, 8, 4
            };
            std::memcpy(&set_queue_header, &buffer[index], sizeof(set_queue_header));
            set_queue_header = boost::endian::big_to_native(set_queue_header);
            index += sizeof(v13_detail::ofp_action_set_queue);
            BOOST_CHECK_EQUAL(set_queue_header.type, expected_set_queue_header.type);
            BOOST_CHECK_EQUAL(set_queue_header.len, expected_set_queue_header.len);
            BOOST_CHECK_EQUAL(set_queue_header.queue_id, expected_set_queue_header.queue_id);
        }
    }
    {
        auto header = v13_detail::ofp_instruction_goto_table{};
        auto expected_header = v13_detail::ofp_instruction_goto_table{
            protocol::OFPIT_GOTO_TABLE, 8, 4, {0, 0, 0}
        };
        std::memcpy(&header, &buffer[index], sizeof(header));
        header = boost::endian::big_to_native(header);
        index += sizeof(header);
        BOOST_CHECK_EQUAL(header.type, expected_header.type);
        BOOST_CHECK_EQUAL(header.len, expected_header.len);
        BOOST_CHECK_EQUAL(header.table_id, expected_header.table_id);
        CANARD_CHECK_EQUAL_COLLECTIONS(header.pad, expected_header.pad);
    }
    BOOST_CHECK_EQUAL(index, buffer.size());
}

BOOST_AUTO_TEST_CASE(decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instruction_set{
          instructions::write_actions{ actions::set_queue{4}, actions::set_ipv4_src{address_v4{0x7f000003}} }
        , instructions::clear_actions{}
        , instructions::apply_actions{ actions::set_eth_type{0x0800}, actions::set_eth_src{mac_address{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}} }
        , instructions::goto_table{4}
    };
    sut.encode(buffer);

    auto it = buffer.begin();
    auto const decoded_set = instruction_set::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_set.length(), sut.length());
}

BOOST_AUTO_TEST_SUITE_END() // instruction_set_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
