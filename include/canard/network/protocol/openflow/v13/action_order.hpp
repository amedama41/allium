#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ORDER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ORDER_HPP

#include <cstdint>
#include <type_traits>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/support/pair.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace action_order_detail {

        using boost::fusion::pair;

        template <std::uint16_t value>
        using priority = std::integral_constant<std::uint16_t, value>;

        using action_set_priority_map = boost::fusion::map<
              pair<actions::copy_ttl_in, priority<0x0100>>
            , pair<actions::pop_vlan, priority<0x0200>>
            , pair<actions::pop_mpls, priority<0x0200>>
            , pair<actions::pop_pbb, priority<0x0200>>
            , pair<actions::push_mpls, priority<0x0300>>
            , pair<actions::push_pbb, priority<0x0400>>
            , pair<actions::push_vlan, priority<0x0500>>
            , pair<actions::copy_ttl_out, priority<0x0600>>
            , pair<actions::decrement_mpls_ttl, priority<0x0700>>
            , pair<actions::decrement_nw_ttl, priority<0x0700>>
            , pair<actions::set_mpls_ttl, priority<0x0800>>
            , pair<actions::set_nw_ttl, priority<0x0800>>
            , pair<actions::set_queue, priority<0x0900>>
            , pair<actions::group, priority<0x0a00>>
            , pair<actions::output, priority<0x0b00>>
        >;

    } // namespace action_order_detail

    template <class Action>
    struct action_set_priority
    {
        static constexpr std::uint16_t value
            = boost::fusion::result_of::value_at_key<
                  action_order_detail::action_set_priority_map, Action
              >::type::value;
    };

    template <class OXMMatchField>
    struct action_set_priority<actions::set_field<OXMMatchField>>
    {
        static constexpr std::uint16_t value = 0x0800;
    };


    template <class Action>
    struct action_order
    {
        using action = Action;

        static constexpr std::uint64_t value
            = (std::uint64_t(action_set_priority<action>::value) << 48)
            | (std::uint64_t(action::type()) << 32);

        static constexpr auto get_value(action const&) noexcept
            -> std::uint64_t
        {
            return value;
        }
    };

    template <class OXMMatchField>
    struct action_order<actions::set_field<OXMMatchField>>
    {
        using action = actions::set_field<OXMMatchField>;

        static constexpr std::uint64_t value
            = (std::uint64_t(action_set_priority<action>::value) << 48)
            | (std::uint64_t(action::type()) << 32)
            | std::uint64_t(action::oxm_type());

        static constexpr auto get_value(action const&) noexcept
            -> std::uint64_t
        {
            return value;
        }
    };

    template <class Action>
    auto get_order(Action const& action)
        -> std::uint64_t
    {
        return action_order<Action>::get_value(action);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_ORDER_HPP
