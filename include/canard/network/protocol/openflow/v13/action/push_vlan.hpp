#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_VLAN_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_VLAN_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v13/detail/basic_action_push.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class push_vlan
        : public detail::v13::basic_action_push<push_vlan>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_PUSH_VLAN;

        explicit push_vlan(std::uint16_t const ethertype) noexcept
            : basic_action_push{ethertype}
        {
        }

        static auto ieee802_1Q() noexcept
            -> push_vlan
        {
            return push_vlan{0x8100};
        }

        static auto ieee802_1ad() noexcept
            -> push_vlan
        {
            return push_vlan{0x88a8};
        }

    private:
        friend basic_action;

        explicit push_vlan(raw_ofp_type const& ofp_action) noexcept
            : basic_action_push{ofp_action}
        {
        }

        static void validate_impl(push_vlan const& action)
        {
            if (action.ethertype() != 0x8100 && action.ethertype() != 0x88a8) {
                throw std::runtime_error{"invalid ethertype"};
            }
        }
    };

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_VLAN_HPP
