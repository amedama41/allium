#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_DECREMENT_MPLS_TTL_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_DECREMENT_MPLS_TTL_HPP

#include <canard/network/protocol/openflow/v13/detail/basic_generic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class decrement_mpls_ttl
            : public action_detail::generic_action<decrement_mpls_ttl>
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_DEC_MPLS_TTL;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_DECREMENT_MPLS_TTL_HPP
