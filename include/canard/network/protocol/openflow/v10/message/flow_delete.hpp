#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_delete
        : public v10_detail::basic_openflow_message<flow_delete>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE;

        explicit flow_delete(
                  match_set const& match
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION, message_type, sizeof(flow_mod_), xid
                  }
                , match.ofp_match(), 0, command_type, 0, 0
                , 0, 0, out_port, 0
              }
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
            return detail::encode(container, flow_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_delete
        {
            return flow_delete{
                detail::decode<v10_detail::ofp_flow_mod>(first, last)
            };
        }

    private:
        explicit flow_delete(v10_detail::ofp_flow_mod const flow_mod)
            : flow_mod_(flow_mod)
        {
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
    };


    class flow_delete_strict
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE_STRICT;

        flow_delete_strict(
                  match_set const& match, std::uint16_t const priority
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION, message_type, sizeof(flow_mod_), xid
                  }
                , match.ofp_match(), 0, command_type, 0, 0
                , priority, 0, out_port, 0
              }
        {
        }

        explicit flow_delete_strict(
                  flow_entry const& entry
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid())
            : flow_mod_{
                  {protocol::OFP_VERSION, message_type, sizeof(flow_mod_), xid}
                , entry.ofp_match(), 0, command_type, 0, 0
                , entry.priority(), 0, out_port, 0
              }
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
            return detail::encode(container, flow_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_delete_strict
        {
            return flow_delete_strict{
                detail::decode<v10_detail::ofp_flow_mod>(first, last)
            };
        }

    private:
        explicit flow_delete_strict(v10_detail::ofp_flow_mod const flow_mod)
            : flow_mod_(flow_mod)
        {
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP
