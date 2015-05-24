#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_REMOVED_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_REMOVED_HPP

#include <cstdint>
#include <chrono>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class flow_removed
        : public v10_detail::basic_openflow_message<flow_removed>
        , public v10_detail::flow_entry_adaptor<flow_removed, v10_detail::ofp_flow_removed>
    {
    public:
        static ofp_type const message_type = OFPT_FLOW_REMOVED;

        auto header() const
            -> v10_detail::ofp_header
        {
            return flow_removed_.header;
        }

        auto reason() const
            -> ofp_flow_removed_reason
        {
            return ofp_flow_removed_reason(flow_removed_.reason);
        }

        auto duration_sec() const
            -> std::chrono::seconds
        {
            return std::chrono::seconds{flow_removed_.duration_sec};
        }

        auto duration_nsec() const
            -> std::chrono::nanoseconds
        {
            return std::chrono::nanoseconds{flow_removed_.duration_nsec};
        }

        auto ofp_flow_entry() const
            -> v10_detail::ofp_flow_removed const&
        {
            return flow_removed_;
        }

        auto is_idle_timeout() const
            -> bool
        {
            return reason() == OFPRR_IDLE_TIMEOUT;
        }

        auto is_hard_timeout() const
            -> bool
        {
            return reason() == OFPRR_HARD_TIMEOUT;
        }

        auto is_deleted() const
            -> bool
        {
            return reason() == OFPRR_DELETE;
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
            auto const removed
                = detail::decode<v10_detail::ofp_flow_removed>(first, last);
            return flow_removed{removed};
        }

    private:
        explicit flow_removed(v10_detail::ofp_flow_removed const& removed)
            : flow_removed_(removed)
        {
            if (version() != v10::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
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
