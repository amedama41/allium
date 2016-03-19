#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP

#include <cstdint>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/detail/any_action.hpp>
#include <canard/network/protocol/openflow/v13/action_order.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/decoder/action_decoder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using any_action = detail::any_action<
        default_all_action_list, action_decoder, protocol
    >;

    template <class T>
    auto any_cast(any_action const& action)
        -> T const&
    {
        return detail::any_cast<T>(action);
    }

    template <class T>
    auto any_cast(any_action const* const action)
        -> T const*
    {
        return detail::any_cast<T>(action);
    }

    namespace any_action_detail {

        struct action_order_visitor
            : boost::static_visitor<std::uint64_t>
        {
            template <class Action>
            auto operator()(Action const& action) const
                -> result_type
            {
                return action_order<Action>::get_value(action);
            }
        };

    } // namespace any_action_detail

    template <>
    struct action_order<any_action>
    {
        static auto get_value(any_action const& action)
            -> std::uint64_t
        {
            auto visitor = any_action_detail::action_order_visitor{};
            return action.visit(visitor);
        }
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP
