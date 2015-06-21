#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_VLAN_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_VLAN_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/action/action_push.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class push_vlan
            : public action_detail::action_push<push_vlan>
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_PUSH_VLAN;

            explicit push_vlan(std::uint16_t const ethertype)
                : action_push{ethertype}
            {
                if (ethertype != 0x8100 && ethertype != 0x88a8) {
                    throw 3;
                }
            }
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_VLAN_HPP
