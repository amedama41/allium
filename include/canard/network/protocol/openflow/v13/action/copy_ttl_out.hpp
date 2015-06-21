#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_COPY_TTL_OUT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_COPY_TTL_OUT_HPP

#include <canard/network/protocol/openflow/v13/action/generic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class copy_ttl_out
            : public action_detail::generic_action<copy_ttl_out>
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_COPY_TTL_OUT;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_COPY_TTL_OUT_HPP
