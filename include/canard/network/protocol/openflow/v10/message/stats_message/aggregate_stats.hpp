#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_AGGREGATE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_AGGREGATE_STATS_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_stats.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace statistics {

    class aggregate_stats_request
        : public stats_detail::basic_stats_request<
              aggregate_stats_request, v10_detail::ofp_aggregate_stats_request
          >
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_AGGREGATE;

        aggregate_stats_request(
                  match_set const& match
                , std::uint8_t const table_id
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid())
            : basic_stats_request{
                  0
                , v10_detail::ofp_aggregate_stats_request{
                      match.ofp_match(), table_id, 0, out_port
                  }
                , xid
              }
        {
        }

    private:
        friend basic_stats_request::base_type;

        aggregate_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_aggregate_stats_request const& aggregate_stats_request) noexcept
            : basic_stats_request{stats_request, aggregate_stats_request}
        {
        }
    };


    class aggregate_stats_reply
        : public stats_detail::basic_stats_reply<
              aggregate_stats_reply, v10_detail::ofp_aggregate_stats_reply
          >
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_AGGREGATE;

        auto packet_count() const noexcept
            -> std::uint64_t
        {
            return body().packet_count;
        }

        auto byte_count() const noexcept
            -> std::uint64_t
        {
            return body().byte_count;
        }

        auto flow_count() const noexcept
            -> std::uint32_t
        {
            return body().flow_count;
        }

    private:
        friend basic_stats_reply::base_type;

        aggregate_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , v10_detail::ofp_aggregate_stats_reply const& aggregate_stats_reply) noexcept
            : basic_stats_reply{stats_reply, aggregate_stats_reply}
        {
        }
    };

} // namespace statistics
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_AGGREGATE_STATS_HPP
