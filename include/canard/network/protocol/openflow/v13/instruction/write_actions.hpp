#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_WRITE_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_WRITE_ACTIONS_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <canard/network/protocol/openflow/v13/action_order.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_instruction_actions.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace instructions {

    class write_actions
        : public detail::v13::basic_instruction_actions<write_actions>
    {
    public:
        static constexpr protocol::ofp_instruction_type instruction_type
            = protocol::OFPIT_WRITE_ACTIONS;

        explicit write_actions(action_list actions)
            : basic_instruction_actions{std::move(actions)}
        {
        }

        explicit write_actions(action_set actions)
            : write_actions{std::move(actions).to_list()}
        {
        }

        template <
              class... Actions
            , typename std::enable_if<
                !canard::is_related<write_actions, Actions...>::value
              >::type* = nullptr
        >
        explicit write_actions(Actions&&... actions)
            : write_actions{action_set{std::forward<Actions>(actions)...}}
        {
        }

    private:
        friend basic_instruction_actions;

        write_actions(
                raw_ofp_type const& instruction_actions, action_list&& actions)
            : basic_instruction_actions{instruction_actions, std::move(actions)}
        {
        }

        static void validate_impl(write_actions const& write_actions)
        {
            auto const& actions = write_actions.actions();
            auto action_order_set = std::unordered_set<std::uint64_t>{};
            action_order_set.reserve(actions.size());
            for (auto const& action : actions) {
                if (!action_order_set.insert(get_order(action)).second) {
                    throw std::runtime_error{"duplicate action"};
                }
            }
        }
    };

} // namespace instructions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_WRITE_ACTIONS_HPP
