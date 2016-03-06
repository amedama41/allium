#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/action/output.hpp>
#include <canard/network/protocol/openflow/v13/action/copy_ttl_out.hpp>
#include <canard/network/protocol/openflow/v13/action/copy_ttl_in.hpp>
#include <canard/network/protocol/openflow/v13/action/set_mpls_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/decrement_mpls_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/push_vlan.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_vlan.hpp>
#include <canard/network/protocol/openflow/v13/action/push_mpls.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_mpls.hpp>
#include <canard/network/protocol/openflow/v13/action/set_queue.hpp>
#include <canard/network/protocol/openflow/v13/action/group.hpp>
#include <canard/network/protocol/openflow/v13/action/set_nw_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/decrement_nw_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/set_field.hpp>
#include <canard/network/protocol/openflow/v13/action/push_pbb.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_pbb.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions_detail {

        template <class Tuple1, class Tuple2>
        struct tuple_cat;

        template <class... Ts1, class... Ts2>
        struct tuple_cat<std::tuple<Ts1...>, std::tuple<Ts2...>>
        {
            using type = std::tuple<Ts1..., Ts2...>;
        };

    } // namespace actions_detail

    using default_action_list = std::tuple<
          actions::output
        , actions::copy_ttl_out
        , actions::copy_ttl_in
        , actions::set_mpls_ttl
        , actions::decrement_mpls_ttl
        , actions::push_vlan
        , actions::pop_vlan
        , actions::push_mpls
        , actions::pop_mpls
        , actions::set_queue
        , actions::group
        , actions::set_nw_ttl
        , actions::decrement_nw_ttl
        , actions::push_pbb
        , actions::pop_pbb
    >;

    using default_all_action_list = actions_detail::tuple_cat<
          default_action_list
        , default_set_field_list
    >::type;

    // TODO: each set_field change independent type
    namespace actions {
        inline auto action_order(copy_ttl_in const&)
            -> std::uint64_t
        {
            return 0x0001000000000000;
        }

        inline auto action_order(pop_vlan const&)
            -> std::uint64_t
        {
            return 0x0002000000000000 | pop_vlan::action_type;
        }

        inline auto action_order(pop_mpls const&)
            -> std::uint64_t
        {
            return 0x0002000000000000 | pop_mpls::action_type;
        }

        inline auto action_order(pop_pbb const&)
            -> std::uint64_t
        {
            return 0x0002000000000000 | pop_pbb::action_type;
        }

        inline auto action_order(push_mpls const&)
            -> std::uint64_t
        {
            return 0x0003000000000000;
        }

        inline auto action_order(push_pbb const&)
            -> std::uint64_t
        {
            return 0x0004000000000000;
        }

        inline auto action_order(push_vlan const&)
            -> std::uint64_t
        {
            return 0x0005000000000000;
        }

        inline auto action_order(copy_ttl_out const&)
            -> std::uint64_t
        {
            return 0x0006000000000000;
        }

        inline auto action_order(decrement_mpls_ttl const&)
            -> std::uint64_t
        {
            return 0x0007000000000000 | decrement_mpls_ttl::action_type;
        }

        inline auto action_order(decrement_nw_ttl const&)
            -> std::uint64_t
        {
            return 0x0007000000000000 | decrement_nw_ttl::action_type;
        }

        inline auto action_order(set_mpls_ttl const&)
            -> std::uint64_t
        {
            return 0x0008000000000000 | set_mpls_ttl::action_type;
        }

        inline auto action_order(set_nw_ttl const&)
            -> std::uint64_t
        {
            return 0x0008000000000000 | set_nw_ttl::action_type;
        }

        template <class OXMMatchField>
        inline auto action_order(set_field<OXMMatchField> const& set_field_action)
            -> std::uint64_t
        {
            return 0x0008000000800000 | OXMMatchField::oxm_type();
        }

        inline auto action_order(set_queue const&)
            -> std::uint64_t
        {
            return 0x0009000000000000;
        }

        inline auto action_order(group const&)
            -> std::uint64_t
        {
            return 0x000a000000000000;
        }

        inline auto action_order(output const&)
            -> std::uint64_t
        {
            return 0x000b000000000000;
        }

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP
