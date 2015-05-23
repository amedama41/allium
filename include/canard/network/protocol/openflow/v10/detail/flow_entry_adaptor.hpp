#ifndef CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_ADAPTOR_HPP

#include <cstdint>
#include <chrono>
#include <canard/network/protocol/openflow/v10/detail/match_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace v10_detail {

    template <class T, class FlowEntry>
    class flow_entry_adaptor
        : public match_adaptor<T>
    {
    public:
        auto match() const
            -> match_set
        {
            return match_set{ofp_match()};
        }

        auto priority() const
            -> std::uint16_t
        {
            return base_flow_entry().priority;
        }

        auto idle_timeout() const
            -> std::uint16_t
        {
            return base_flow_entry().idle_timeout;
        }

        auto hard_timeout() const
            -> std::uint16_t
        {
            return base_flow_entry().hard_timeout;
        }

        auto cookie() const
            -> std::uint64_t
        {
            return base_flow_entry().cookie;
        }

        auto packet_count() const
            -> std::uint64_t
        {
            return base_flow_entry().packet_count;
        }

        auto byte_count() const
            -> std::uint64_t
        {
            return base_flow_entry().byte_count;
        }

        auto ofp_match() const
            -> v10_detail::ofp_match const&
        {
            return base_flow_entry().match;
        }

    private:
        auto base_flow_entry() const
            -> FlowEntry const&
        {
            return static_cast<T const*>(this)->ofp_flow_entry();
        }
    };

} // namespace v10_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_ADAPTOR_HPP
