#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_ADD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_ADD_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v13/detail/flow_mod_base.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class flow_add
        : public flow_mod_detail::flow_mod_base<flow_add>
        , public v13_detail::flow_entry_adaptor<
                flow_add, v13_detail::ofp_flow_mod
          >
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_ADD;

        flow_add(flow_entry&& entry
               , std::uint8_t const table_id
               , v13::timeouts const& timeouts
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_mod_base{
                  std::move(entry).match()
                , entry.priority()
                , entry.cookie()
                , 0
                , std::move(entry).instructions()
                , table_id
                , timeouts.idle_timeout()
                , timeouts.hard_timeout()
                , flags
                , buffer_id
                , xid
              }
        {
        }

        flow_add(flow_entry const& entry
               , std::uint8_t const table_id
               , v13::timeouts const& timeouts
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_add{
                flow_entry(entry), table_id, timeouts, flags, buffer_id, xid
              }
        {
        }

        flow_add(flow_entry entry
               , std::uint8_t const table_id
               , std::uint16_t const flags = 0
               , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
               , std::uint32_t const xid = get_xid())
            : flow_add{
                  std::move(entry)
                , table_id
                , v13::timeouts{0, 0}
                , flags
                , buffer_id
                , xid
              }
        {
        }

        auto entry() const
            -> flow_entry
        {
            return flow_entry{match(), priority(), cookie(), instructions()};
        }

    private:
        friend flow_mod_base;

        flow_add(v13_detail::ofp_flow_mod const& flow_mod
               , oxm_match&& match
               , instruction_set&& instructions)
            : flow_mod_base{
                flow_mod, std::move(match), std::move(instructions)
              }
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v13_detail::ofp_flow_mod const&
        {
            return ofp_flow_mod();
        }
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_ADD_HPP
