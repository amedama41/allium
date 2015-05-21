#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MOD_ADD_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MOD_ADD_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_mod_add
        : public v10_detail::basic_openflow_message<flow_mod_add>
    {
    public:
        static ofp_type const message_type = OFPT_FLOW_MOD;
        static ofp_flow_mod_command const command_type = OFPFC_ADD;

        flow_mod_add(flow_entry entry
                , std::uint16_t const flags, std::uint32_t const buffer_id
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  {OFP_VERSION, message_type, std::uint16_t(sizeof(flow_mod_) + entry.actions().length()), xid}
                , entry.match().ofp_match(), entry.cookie(), command_type
                , entry.idle_timeout(), entry.hard_timeout()
                , entry.priority(), buffer_id, 0, flags
              }
            , entry_(std::move(entry))
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return flow_mod_.header;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_mod_);
            return entry_.actions().encode(container);
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
        flow_entry entry_;
    };

} // namespace messages
} // namespace v10
} // namespace openfn
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MOD_ADD_HPP
