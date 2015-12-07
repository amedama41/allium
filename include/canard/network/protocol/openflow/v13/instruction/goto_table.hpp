#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_GOTO_TABLE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_GOTO_TABLE_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class goto_table
        {
        public:
            static protocol::ofp_instruction_type const instruction_type
                = protocol::OFPIT_GOTO_TABLE;

            explicit goto_table(std::uint8_t const table_id)
                : goto_table_{instruction_type, sizeof(v13_detail::ofp_instruction_goto_table), table_id, {0}}
            {
                if (table_id > protocol::OFPTT_MAX) {
                    throw 3;
                }
            }

            auto type() const
                -> protocol::ofp_instruction_type
            {
                return instruction_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_instruction_goto_table);
            }

            auto table_id() const
                -> std::uint8_t
            {
                return goto_table_.table_id;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return v13_detail::encode(container, goto_table_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> goto_table
            {
                auto const instruction_goto_table = v13_detail::decode<v13_detail::ofp_instruction_goto_table>(first, last);
                if (instruction_goto_table.type != instruction_type) {
                    throw 1;
                }
                if (instruction_goto_table.len != sizeof(v13_detail::ofp_instruction_goto_table)) {
                    throw 2;
                }
                return goto_table{instruction_goto_table.table_id};
            }

        private:
            v13_detail::ofp_instruction_goto_table goto_table_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_GOTO_TABLE_HPP
