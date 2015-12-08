#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_REMOVED_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_REMOVED_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class flow_removed
        : public v13_detail::basic_openflow_message<flow_removed>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_FLOW_REMOVED;

        flow_removed(
                  flow_entry const& entry
                , std::uint8_t const table_id
                , protocol::ofp_flow_removed_reason const reason
                , std::uint32_t const duration_sec
                , std::uint32_t const duration_nsec)
            : flow_removed_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , v13_detail::exact_length(sizeof(v13_detail::ofp_flow_removed) + entry.match().length())
                    , get_xid()
                  }
                , entry.cookie()
                , entry.priority()
                , std::uint8_t(reason)
                , table_id
                , duration_sec
                , duration_nsec
                , entry.idle_timeout()
                , entry.hard_timeout()
                , entry.packet_count()
                , entry.byte_count()
              }
            , match_(entry.match())
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return flow_removed_.header;
        }

        auto reason() const
            -> protocol::ofp_flow_removed_reason
        {
            return protocol::ofp_flow_removed_reason(flow_removed_.reason);
        }

        auto table_id() const
            -> std::uint8_t
        {
            return flow_removed_.table_id;
        }

        auto duration_sec() const
            -> std::uint32_t
        {
            return flow_removed_.duration_sec;
        }

        auto duration_nsec() const
            -> std::uint32_t
        {
            return flow_removed_.duration_nsec;
        }

        auto match() const
            -> oxm_match const&
        {
            return match_;
        }

        auto priority() const
            -> std::uint16_t
        {
            return flow_removed_.priority;
        }

        auto entry() const
            -> flow_entry
        {
            return flow_entry{{match_, flow_removed_.priority}, instruction_set{}
                , {flow_removed_.packet_count, flow_removed_.byte_count}
                , {flow_removed_.idle_timeout, flow_removed_.hard_timeout}
                , flow_removed_.cookie
            };
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_removed_);
            return match_.encode(container);
        }

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_removed
        {
            auto const flow_rm = detail::decode<v13_detail::ofp_flow_removed>(first, last);
            if (std::distance(first, last) != flow_rm.header.length - sizeof(v13_detail::ofp_flow_removed)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            return flow_removed{flow_rm, std::move(match)};
        }

    private:
        flow_removed(v13_detail::ofp_flow_removed const& flow_rm, oxm_match match)
            : flow_removed_(flow_rm)
            , match_(std::move(match))
        {
        }

    private:
        v13_detail::ofp_flow_removed flow_removed_;
        oxm_match match_;
    };

} // namespace messages

using messages::flow_removed;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_REMOVED_HPP
