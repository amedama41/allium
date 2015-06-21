#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP

#include <canard/network/protocol/openflow/v10/detail/action_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    class strip_vlan
        : public v10_detail::action_adaptor<strip_vlan, v10_detail::ofp_action_header>
    {
        using ofp_action_t = v10_detail::ofp_action_header;

    public:
        static protocol::ofp_action_type const action_type
            = protocol::OFPAT_STRIP_VLAN;

        strip_vlan()
            : strip_vlan_{action_type, sizeof(ofp_action_t), {0}}
        {
        }

    private:
        friend action_adaptor;

        auto ofp_action() const
            -> ofp_action_t const&
        {
            return strip_vlan_;
        }

        explicit strip_vlan(v10_detail::ofp_action_header const action_header)
            : strip_vlan_(action_header)
        {
        }

    private:
        ofp_action_t strip_vlan_;
    };

    using pop_vlan = strip_vlan;

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP
