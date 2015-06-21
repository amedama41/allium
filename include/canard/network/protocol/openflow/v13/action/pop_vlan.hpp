#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_VLAN_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_VLAN_HPP

#include <canard/network/protocol/openflow/v13/action/generic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class pop_vlan
            : public action_detail::generic_action<pop_vlan>
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_POP_VLAN;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_VLAN_HPP
