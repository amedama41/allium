#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_ADAPTOR_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace v13_detail {

    template <class T, class FlowEntry>
    class flow_entry_adaptor
    {
    public:
        auto table_id() const noexcept
            -> std::uint8_t
        {
            return base_flow_entry().table_id;
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return base_flow_entry().priority;
        }

        auto id() const
            -> v13::flow_entry_id
        {
            return v13::flow_entry_id{
                static_cast<T const*>(this)->match(), priority()
            };
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return base_flow_entry().cookie;
        }

        auto duration_sec() const noexcept
            -> std::uint32_t
        {
            return base_flow_entry().duration_sec;
        }

        auto duration_nsec() const noexcept
            -> std::uint32_t
        {
            return base_flow_entry().duration_nsec;
        }

        auto elapsed_time() const noexcept
            -> v13::elapsed_time
        {
            return v13::elapsed_time{duration_sec(), duration_nsec()};
        }

        auto idle_timeout() const noexcept
            -> std::uint16_t
        {
            return base_flow_entry().idle_timeout;
        }

        auto hard_timeout() const noexcept
            -> std::uint16_t
        {
            return base_flow_entry().hard_timeout;
        }

        auto timeouts() const noexcept
            -> v13::timeouts
        {
            return v13::timeouts{idle_timeout(), hard_timeout()};
        }

        auto packet_count() const noexcept
            -> std::uint64_t
        {
            return base_flow_entry().packet_count;
        }

        auto byte_count() const noexcept
            -> std::uint64_t
        {
            return base_flow_entry().byte_count;
        }

        auto counters() const noexcept
            -> v13::counters
        {
            return v13::counters{packet_count(), byte_count()};
        }

        auto flags() const noexcept
            -> std::uint16_t
        {
            return base_flow_entry().flags;
        }

    private:
        auto base_flow_entry() const noexcept
            -> FlowEntry const&
        {
            return static_cast<T const*>(this)->ofp_flow_entry();
        }
    };

} // namespace v13_detail
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_ADAPTOR_HPP
