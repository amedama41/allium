#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_APPLY_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_APPLY_ACTIONS_HPP

#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class apply_actions
        {
        public:
            static ofp_instruction_type const instruction_type = OFPIT_APPLY_ACTIONS;

            explicit apply_actions(action_list act_list)
                : actions_{instruction_type, std::uint16_t(sizeof(detail::ofp_instruction_actions) + act_list.length()), {0, 0, 0, 0}}
                , action_list_(std::move(act_list))
            {
            }

            template <class... Actions, typename std::enable_if<!is_related<apply_actions, Actions...>::value>::type* = nullptr>
            apply_actions(Actions&&... actions)
                : actions_{instruction_type, actions_length(actions...), {0, 0, 0, 0}}
                , action_list_{std::forward<Actions>(actions)...}
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
                return actions_.len;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, actions_);
                return action_list_.encode(container);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> apply_actions
            {
                auto const instruction_actions = detail::decode<detail::ofp_instruction_actions>(first, last);
                if (instruction_actions.type != instruction_type) {
                    throw 1;
                }
                if (instruction_actions.len < sizeof(detail::ofp_instruction_actions)) {
                    throw 2;
                }
                if (std::distance(first, last) < instruction_actions.len - sizeof(detail::ofp_instruction_actions)) {
                    throw 2;
                }
                auto act_list = action_list::decode(first, std::next(first, instruction_actions.len - sizeof(detail::ofp_instruction_actions)));
                return apply_actions{std::move(act_list)};
            }

        private:
            static auto actions_length()
                -> std::uint16_t
            {
                return sizeof(detail::ofp_instruction_actions);
            }

            template <class Action, class... Actions>
            static auto actions_length(Action const& action, Actions const&... actions)
                -> std::uint16_t
            {
                return action.length() + actions_length(actions...);
            }

        private:
            detail::ofp_instruction_actions actions_;
            action_list action_list_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_APPLY_ACTIONS_HPP
