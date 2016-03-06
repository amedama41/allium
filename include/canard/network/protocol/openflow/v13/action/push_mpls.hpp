#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_MPLS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_MPLS_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v13/detail/basic_action_push.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class push_mpls
        : public detail::v13::basic_action_push<push_mpls>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_PUSH_MPLS;

        explicit push_mpls(std::uint16_t const ethertype) noexcept
            : basic_action_push{ethertype}
        {
        }

        static auto unicast() noexcept
            -> push_mpls
        {
            return push_mpls{0x8847};
        }

        static auto multicast() noexcept
            -> push_mpls
        {
            return push_mpls{0x8848};
        }

    private:
        friend basic_action;

        explicit push_mpls(raw_ofp_type const& ofp_action) noexcept
            : basic_action_push{ofp_action}
        {
        }

        static void validate_impl(push_mpls const& action)
        {
            if (action.ethertype() != 0x8847 && action.ethertype() != 0x8848) {
                throw std::runtime_error{"invalid ethertype"};
            }
        }
    };

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_PUSH_MPLS_HPP
