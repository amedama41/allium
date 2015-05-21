#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_ACTIONS_HPP

#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/action_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class write_actions
        {
        public:
            static ofp_instruction_type const instruction_type = OFPIT_WRITE_ACTIONS;

            explicit write_actions(action_set act_set)
                : actions_{instruction_type, std::uint16_t(sizeof(v13_detail::ofp_instruction_actions) + act_set.length()), {0, 0, 0, 0}}
                , action_set_(std::move(act_set))
            {
            }

            template <class... Actions, typename std::enable_if<!is_related<write_actions, Actions...>::value>::type* = nullptr>
            write_actions(Actions&&... actions)
                : write_actions{action_set{std::forward<Actions>(actions)...}}
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
                return action_set_.encode(container);
            }

        private:
            write_actions(action_set actions, std::uint16_t const length)
                : actions_{instruction_type, length, {0, 0, 0, 0}}
                , action_set_(std::move(actions))
            {
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> write_actions
            {
                auto const instruction_actions = detail::decode<v13_detail::ofp_instruction_actions>(first, last);
                if (instruction_actions.len < sizeof(v13_detail::ofp_instruction_actions)) {
                    throw 2;
                }
                if (std::distance(first, last) < instruction_actions.len - sizeof(v13_detail::ofp_instruction_actions)) {
                    throw 2;
                }
                auto act_set = action_set::decode(first, std::next(first, instruction_actions.len - sizeof(v13_detail::ofp_instruction_actions)));
                return write_actions{std::move(act_set), instruction_actions.len};
            }

        private:
            v13_detail::ofp_instruction_actions actions_;
            action_set action_set_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_ACTIONS_HPP
