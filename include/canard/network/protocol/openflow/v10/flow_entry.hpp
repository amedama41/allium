#ifndef CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    struct flow_entry_id
    {
        match_set match_;
        std::uint16_t priority_;
    };

    struct counters
    {
        std::uint64_t packet_count_;
        std::uint64_t byte_count_;
    };

    struct timeouts
    {
        std::uint16_t idle_timeout_;
        std::uint16_t hard_timeout_;
    };

    class flow_entry
    {
    public:
        flow_entry(flow_entry_id const& id, action_list actions)
            : flow_entry{id, std::move(actions), {0, 0}, {OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT}, 0}
        {
        }

        flow_entry(flow_entry_id const& id, action_list actions
                , counters const counters
                , timeouts const timeouts
                , std::uint64_t const cookie)
            : identifier_(id)
            , actions_(std::move(actions))
            , counters_(counters)
            , timeouts_(timeouts)
            , cookie_(cookie)
        {
        }

        auto match() const
            -> match_set const&
        {
            return identifier_.match_;
        }

        auto priority() const
            -> std::uint16_t
        {
            return identifier_.priority_;
        }

        auto idle_timeout() const
            -> std::uint16_t
        {
            return timeouts_.idle_timeout_;
        }

        auto hard_timeout() const
            -> std::uint16_t
        {
            return timeouts_.hard_timeout_;
        }

        auto cookie() const
            -> std::uint64_t
        {
            return cookie_;
        }

        auto actions() const
            -> action_list const&
        {
            return actions_;
        }

    private:
        flow_entry_id identifier_;
        action_list actions_;

        counters counters_;
        timeouts timeouts_;
        std::uint64_t cookie_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
