#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    struct flow_entry_id
    {
        oxm_match match_;
        std::uint16_t priority_;

        static auto table_miss()
            -> flow_entry_id
        {
            return {oxm_match{}, 0};
        }
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
        flow_entry(flow_entry_id identifer, instruction_set instructions)
            : flow_entry{
                  std::move(identifer), std::move(instructions), {0, 0}
                , {protocol::OFP_FLOW_PERMANENT, protocol::OFP_FLOW_PERMANENT}
                , 0
              }
        {
        }

        flow_entry(flow_entry_id identifer, instruction_set instructions, timeouts const timeouts)
            : flow_entry{std::move(identifer), std::move(instructions), {0, 0}, timeouts, 0}
        {
        }

        flow_entry(flow_entry_id identifer, instruction_set instructions, std::uint64_t const cookie)
            : flow_entry{
                  std::move(identifer), std::move(instructions), {0, 0}
                , {protocol::OFP_FLOW_PERMANENT, protocol::OFP_FLOW_PERMANENT}
                , cookie
              }
        {
        }

        flow_entry(flow_entry_id identifer, instruction_set instructions, timeouts const timeouts, std::uint64_t const cookie)
            : flow_entry{std::move(identifer), std::move(instructions), {0, 0}, timeouts, cookie}
        {
        }

        flow_entry(flow_entry_id identifer, instruction_set instructions
                , counters const counters, timeouts const timeouts, std::uint64_t const cookie)
            : identifier_(std::move(identifer))
            , instructions_(std::move(instructions))
            , counters_(counters)
            , timeouts_(timeouts)
            , cookie_{cookie}
        {
        }

        auto id() const
            -> flow_entry_id const&
        {
            return identifier_;
        }

        auto match() const
            -> oxm_match const&
        {
            return identifier_.match_;
        }

        auto priority() const
            -> std::uint16_t
        {
            return identifier_.priority_;
        }

        auto instructions() const
            -> instruction_set const&
        {
            return instructions_;
        }

        auto packet_count() const
            -> std::uint64_t
        {
            return counters_.packet_count_;
        }

        auto byte_count() const
            -> std::uint64_t
        {
            return counters_.byte_count_;
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

    private:
        flow_entry_id identifier_;
        instruction_set instructions_;

        counters counters_;
        timeouts timeouts_;
        std::uint64_t cookie_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_ENTRY_HPP
