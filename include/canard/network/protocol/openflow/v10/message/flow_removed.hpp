#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_REMOVED_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_REMOVED_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_removed
        : public v10_detail::basic_openflow_message<flow_removed>
        , public v10_detail::flow_entry_adaptor<
                flow_removed, v10_detail::ofp_flow_removed
          >
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FLOW_REMOVED;

        flow_removed(
                  match_set const& match
                , std::uint16_t const priority
                , std::uint64_t const cookie
                , protocol::ofp_flow_removed_reason const reason
                , v10::elapsed_time const elapsed_time
                , v10::timeouts const& timeouts
                , v10::counters const& counters
                , std::uint32_t const xid = get_xid())
            : flow_removed_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , sizeof(v10_detail::ofp_flow_removed)
                    , xid
                  }
                , match.ofp_match()
                , cookie
                , priority
                , std::uint8_t(reason)
                , { 0 }
                , elapsed_time.duration_sec()
                , elapsed_time.duration_nsec()
                , timeouts.idle_timeout()
                , { 0, 0 }
                , counters.packet_count()
                , counters.byte_count()
              }
        {
        }

        flow_removed(
                  flow_entry const& entry
                , protocol::ofp_flow_removed_reason const reason
                , v10::elapsed_time const elapsed_time
                , v10::timeouts const& timeouts
                , v10::counters const& counters
                , std::uint32_t const xid = get_xid())
            : flow_removed{
                  entry.match(), entry.priority(), entry.cookie()
                , reason
                , elapsed_time
                , timeouts
                , counters
                , xid
              }
        {
        }

        auto header() const noexcept
            -> v10_detail::ofp_header const&
        {
            return flow_removed_.header;
        }

        auto reason() const noexcept
            -> protocol::ofp_flow_removed_reason
        {
            return protocol::ofp_flow_removed_reason(flow_removed_.reason);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, flow_removed_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_removed
        {
            return flow_removed{
                detail::decode<v10_detail::ofp_flow_removed>(first, last)
            };
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length != sizeof(v10_detail::ofp_flow_removed)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit flow_removed(
                v10_detail::ofp_flow_removed const& removed) noexcept
            : flow_removed_(removed)
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v10_detail::ofp_flow_removed const&
        {
            return flow_removed_;
        }

    private:
        v10_detail::ofp_flow_removed flow_removed_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_REMOVED_HPP
