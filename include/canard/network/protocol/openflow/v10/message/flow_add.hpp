#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_mod_base.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_add
        : public flow_mod_detail::flow_mod_base<flow_add>
        , public v10_detail::flow_entry_adaptor<
                flow_add, v10_detail::ofp_flow_mod
          >
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_ADD;

        flow_add(flow_entry&& entry
               , v10::timeouts const& timeouts
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_mod_base{
                  entry.match()
                , entry.priority()
                , entry.cookie()
                , std::move(entry).actions()
                , timeouts.idle_timeout()
                , timeouts.hard_timeout()
                , flags
                , buffer_id
                , xid
              }
        {
        }

        flow_add(flow_entry const& entry
               , v10::timeouts const& timeouts
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_add{flow_entry(entry), timeouts, flags, buffer_id, xid}
        {
        }

        flow_add(flow_entry entry
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_add{
                  std::move(entry)
                , v10::timeouts{0, 0}
                , flags
                , buffer_id
                , xid
              }
        {
        }

        auto entry() const
            -> flow_entry
        {
            return flow_entry{match(), priority(), cookie(), actions()};
        }

    private:
        friend flow_mod_base;

        flow_add(v10_detail::ofp_flow_mod const& flow_mod
               , action_list&& actions)
            : flow_mod_base{flow_mod, std::move(actions)}
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v10_detail::ofp_flow_mod const&
        {
            return ofp_flow_mod();
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_ADD_HPP
