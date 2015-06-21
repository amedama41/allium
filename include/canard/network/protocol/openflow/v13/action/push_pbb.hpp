#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_PBB_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_PBB_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/action/action_push.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class push_pbb
            : public action_detail::action_push<push_pbb>
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_PUSH_PBB;

            push_pbb()
                : action_push{0x88e7}
            {
            }

        private:
            friend action_push;

            push_pbb(std::uint16_t const ethertype)
                : action_push{ethertype}
            {
                if (ethertype != 0x88e7) {
                    throw 3;
                }
            }
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_PUSH_PBB_HPP
