#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_modify
        : public v10_detail::basic_openflow_message<flow_modify>
    {
    public:
        static ofp_type const message_type = OFPT_FLOW_MOD;
        static ofp_flow_mod_command const command_type = OFPFC_MODIFY;

        flow_modify(
                  match_set const& match, action_list actions
                , std::uint64_t const cookie = 0
                , std::uint32_t const buffer_id = OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      OFP_VERSION, message_type
                    , std::uint16_t(sizeof(flow_mod_) + actions.length())
                    , xid
                  }
                , match.ofp_match(), cookie, command_type
                , 0, 0, 0, buffer_id, OFPP_NONE, 0
              }
            , actions_(std::move(actions))
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
            -> flow_modify
        {
            auto const flow_mod
                = detail::decode<v10_detail::ofp_flow_mod>(first, last);
            auto actions = action_list::decode(first, last);
            return flow_modify{flow_mod, std::move(actions)};
        }

    private:
        flow_modify(v10_detail::ofp_flow_mod const& flow_mod, action_list actions)
            : flow_mod_(flow_mod), actions_(std::move(actions))
        {
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
        action_list actions_;
    };


    class flow_modify_strict
        : public v10_detail::basic_openflow_message<flow_modify_strict>
    {
    public:
        static ofp_type const message_type = OFPT_FLOW_MOD;
        static ofp_flow_mod_command const command_type = OFPFC_MODIFY_STRICT;

        flow_modify_strict(
                  match_set const& match, std::uint16_t const priority
                , action_list actions
                , std::uint64_t const cookie = 0
                , std::uint32_t const buffer_id = OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      OFP_VERSION, message_type
                    , std::uint16_t(sizeof(flow_mod_) + actions.length())
                    , xid
                  }
                , match.ofp_match(), cookie, command_type
                , 0, 0, priority, buffer_id, OFPP_NONE, 0
              }
            , actions_(std::move(actions))
        {
        }

        flow_modify_strict(
                  flow_entry const& entry, action_list actions
                , std::uint32_t const buffer_id = OFP_NO_BUFFER
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      OFP_VERSION, message_type
                    , std::uint16_t(sizeof(flow_mod_) + actions.length())
                    , xid
                  }
                , entry.ofp_match(), entry.cookie(), command_type, 0, 0
                , entry.priority(), buffer_id, OFPP_NONE, 0
              }
            , actions_(std::move(actions))
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
            -> flow_modify_strict
        {
            auto const flow_mod
                = detail::decode<v10_detail::ofp_flow_mod>(first, last);
            auto actions = action_list::decode(first, last);
            return flow_modify_strict{flow_mod, std::move(actions)};
        }

    private:
        flow_modify_strict(
                v10_detail::ofp_flow_mod const& flow_mod, action_list actions)
            : flow_mod_(flow_mod), actions_(std::move(actions))
        {
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
        action_list actions_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_MODIFY_HPP
