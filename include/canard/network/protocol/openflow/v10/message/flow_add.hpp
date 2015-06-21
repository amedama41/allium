#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP

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

    class flow_add
        : public v10_detail::basic_openflow_message<flow_add>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_ADD;

        explicit flow_add(flow_entry entry
                , std::uint16_t const flags
                , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , std::uint16_t(sizeof(flow_mod_) + entry.actions().length())
                    , xid
                  }
                , entry.ofp_match(), entry.cookie(), command_type
                , entry.idle_timeout(), entry.hard_timeout()
                , entry.priority(), buffer_id, 0, flags
              }
            , actions_(std::move(entry).actions())
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
            return actions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_add
        {
            auto const flow_mod
                = detail::decode<v10_detail::ofp_flow_mod>(first, last);
            auto actions = action_list::decode(first, last);
            return flow_add{flow_mod, std::move(actions)};
        }

    private:
        flow_add(v10_detail::ofp_flow_mod const& flow_mod, action_list actions)
            : flow_mod_(flow_mod)
            , actions_(std::move(actions))
        {
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
        action_list actions_;
    };

} // namespace messages
} // namespace v10
} // namespace openfn
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP
