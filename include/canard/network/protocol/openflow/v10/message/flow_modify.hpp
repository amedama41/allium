#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_mod_base.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_modify
        : public flow_mod_detail::flow_mod_base<flow_modify>
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_MODIFY;

        flow_modify(
                  match_set const& match
                , std::uint64_t const cookie
                , action_list actions
                , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_base{
                match, 0, cookie, std::move(actions), 0, 0, 0, buffer_id, xid
              }
        {
        }

        auto match() const noexcept
            -> match_set
        {
            return match_set{ofp_flow_mod().match};
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return ofp_flow_mod().cookie;
        }

    private:
        friend flow_mod_base;

        flow_modify(v10_detail::ofp_flow_mod const& flow_mod
                  , action_list&& actions)
            : flow_mod_base{flow_mod, std::move(actions)}
        {
        }
    };


    class flow_modify_strict
        : public flow_mod_detail::flow_mod_base<flow_modify_strict>
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_MODIFY_STRICT;

        flow_modify_strict(
                  match_set const& match
                , std::uint16_t const priority
                , action_list actions
                , std::uint64_t const cookie = 0
                , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_base{
                  match
                , priority
                , cookie
                , std::move(actions)
                , 0
                , 0
                , 0
                , buffer_id
                , xid
              }
        {
        }

        flow_modify_strict(
                  flow_entry const& entry, action_list actions
                , std::uint32_t const buffer_id = protocol::OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_base{
                  entry.match()
                , entry.priority()
                , entry.cookie()
                , std::move(actions)
                , 0
                , 0
                , 0
                , buffer_id
                , xid
              }
        {
        }

        auto match() const noexcept
            -> match_set
        {
            return match_set{ofp_flow_mod().match};
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return ofp_flow_mod().priority;
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return ofp_flow_mod().cookie;
        }

    private:
        friend flow_mod_base;

        flow_modify_strict(
                v10_detail::ofp_flow_mod const& flow_mod, action_list&& actions)
            : flow_mod_base{flow_mod, std::move(actions)}
        {
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP
