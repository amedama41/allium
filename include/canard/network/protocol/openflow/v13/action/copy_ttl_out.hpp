#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_COPY_TTL_OUT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_COPY_TTL_OUT_HPP

#include <canard/network/protocol/openflow/v13/detail/basic_generic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class copy_ttl_out
        : public detail::v13::basic_generic_action<copy_ttl_out>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_COPY_TTL_OUT;

        copy_ttl_out() = default;

    private:
        friend basic_action;

        explicit copy_ttl_out(raw_ofp_type const& ofp_action) noexcept
            : basic_generic_action{ofp_action}
        {
        }
    };

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_COPY_TTL_OUT_HPP
