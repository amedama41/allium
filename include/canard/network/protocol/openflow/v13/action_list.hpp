#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP

#include <canard/network/protocol/openflow/detail/action_list.hpp>
#include <canard/network/protocol/openflow/v13/any_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using action_list
        = detail::action_list<any_action, v13_detail::ofp_action_header>;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP
