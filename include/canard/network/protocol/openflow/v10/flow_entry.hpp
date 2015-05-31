#ifndef CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v10/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/detail/match_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace flow_entry_detail {

        struct flow_entry
        {
        public:
            v10_detail::ofp_match match;
            std::uint16_t priority;
            std::uint16_t idle_timeout;
            std::uint16_t hard_timeout;
            std::uint64_t cookie;
            std::uint64_t packet_count;
            std::uint64_t byte_count;
        };

    } // namespace flow_entry_detail

    class flow_entry_id
        : public v10_detail::match_adaptor<flow_entry_id>
    {
    public:
        flow_entry_id(match_set const& match, std::uint16_t const priority)
            : match_(match.ofp_match()), priority_(priority)
        {
        }

        auto match() const
            -> match_set
        {
            return match_set{ofp_match()};
        }

        auto priority() const
            -> std::uint16_t
        {
            return priority_;
        }

        auto ofp_match() const
            -> v10_detail::ofp_match const&
        {
            return match_;
        }

    private:
        v10_detail::ofp_match match_;
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
        : public v10_detail::flow_entry_adaptor<flow_entry, flow_entry_detail::flow_entry>
    {
    public:
        flow_entry(flow_entry_id const& id, action_list actions)
            : flow_entry{
                  id
                , std::move(actions)
                , {0, 0}
                , {OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT}
                , 0
              }
        {
        }

        flow_entry(flow_entry_id const& id, action_list actions
                , counters const counters
                , timeouts const timeouts
                , std::uint64_t const cookie)
            : flow_entry_{
                  id.ofp_match(), id.priority()
                , timeouts.idle_timeout_, timeouts.hard_timeout_
                , cookie
                , counters.packet_count_, counters.byte_count_
              }
            , actions_(std::move(actions))
        {
        }

        auto actions() const&
            -> action_list const&
        {
            return actions_;
        }

        auto actions() &&
            -> action_list&&
        {
            return std::move(actions_);
        }

        auto ofp_flow_entry() const
            -> flow_entry_detail::flow_entry const&
        {
            return flow_entry_;
        }

    private:
        flow_entry_detail::flow_entry flow_entry_;
        action_list actions_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
