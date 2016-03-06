#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_PBB_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_PBB_HPP

#include <canard/network/protocol/openflow/v13/detail/basic_generic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class pop_pbb
        : public detail::v13::basic_generic_action<pop_pbb>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_POP_PBB;

        pop_pbb() = default;

    private:
        friend basic_action;

        explicit pop_pbb(raw_ofp_type const& ofp_action) noexcept
            : basic_generic_action{ofp_action}
        {
        }
    };

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_PBB_HPP
