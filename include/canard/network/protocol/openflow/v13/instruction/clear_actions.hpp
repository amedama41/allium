#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CLEAR_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CLEAR_ACTIONS_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class clear_actions
        {
        public:
            static ofp_instruction_type const instruction_type = OFPIT_CLEAR_ACTIONS;

            clear_actions()
                : actions_{instruction_type, sizeof(v13_detail::ofp_instruction_actions), {0}}
            {
            }

            auto type() const
                -> ofp_instruction_type
            {
                return instruction_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_instruction_actions);
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, actions_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> clear_actions
            {
                auto const instruction_actions = detail::decode<v13_detail::ofp_instruction_actions>(first, last);
                if (instruction_actions.type != instruction_type) {
                    throw 1;
                }
                if (instruction_actions.len != sizeof(v13_detail::ofp_instruction_actions)) {
                    throw 2;
                }
                return clear_actions{};
            }

        private:
            v13_detail::ofp_instruction_actions actions_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CLEAR_ACTIONS_HPP
