#ifndef CANARD_NETWORK_OPENFLOW_V13_DEFAULT_ACTION_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DEFAULT_ACTION_LIST_HPP

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
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

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
        , actions::set_field
        , actions::push_pbb
        , actions::pop_pbb
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DEFAULT_ACTION_LIST_HPP
