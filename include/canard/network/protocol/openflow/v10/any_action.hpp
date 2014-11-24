#ifndef CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v10/actions.hpp>
#include <canard/network/protocol/openflow/v10/detail/decode_action.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class any_action
    {
        using action_variant = boost::make_variant_over<
            default_action_list
        >::type;

    public:
        template <
              class Action
            , typename std::enable_if<!canard::is_related<any_action, Action>::value>::type* = nullptr
        >
        any_action(Action&& action)
            : variant_(std::forward<Action>(action))
        {
        }

        template <
              class Action
            , typename std::enable_if<!canard::is_related<any_action, Action>::value>::type* = nullptr
        >
        auto operator=(Action&& action)
            -> any_action&
        {
            variant_ = std::forward<Action>(action);
            return *this;
        }

        auto type() const
            -> ofp_action_type
        {
            auto visitor = openflow::detail::type_visitor<ofp_action_type>{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto length() const
            -> std::uint16_t
        {
            auto visitor = openflow::detail::length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto visitor = openflow::detail::encoding_visitor<Container>{container};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> any_action
        {
            return detail::decode_action<any_action>(first, last, to_any_action{});
        }

        template <class T>
        friend auto any_cast(any_action const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_action const*)
            -> T const*;

    private:
        struct to_any_action
        {
            template <class Action>
            auto operator()(Action&& action) const
                -> any_action
            {
                return any_action{std::forward<Action>(action)};
            }
        };

    private:
        action_variant variant_;
    };

    template <class T>
    auto any_cast(any_action const& action)
        -> T const&
    {
        return boost::get<T>(action.variant_);
    }

    template <class T>
    auto any_cast(any_action const* action)
        -> T const*
    {
        return boost::get<T>(&action->variant_);
    }

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP
