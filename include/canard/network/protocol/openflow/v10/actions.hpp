#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/action/enqueue.hpp>
#include <canard/network/protocol/openflow/v10/action/output.hpp>
#include <canard/network/protocol/openflow/v10/action/set_field.hpp>
#include <canard/network/protocol/openflow/v10/action/strip_vlan.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_action_list = std::tuple<
          actions::output
        , actions::set_vlan_vid
        , actions::set_vlan_pcp
        , actions::strip_vlan
        , actions::set_eth_src
        , actions::set_eth_dst
        , actions::set_ipv4_tos
        , actions::set_ipv4_src
        , actions::set_ipv4_dst
        , actions::set_tcp_dst
        , actions::set_tcp_src
        , actions::enqueue
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP
