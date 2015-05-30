#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_AGGREGATE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_AGGREGATE_STATS_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/stats_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class aggregate_stats_request
        : public v10_detail::stats_request_adaptor<
                aggregate_stats_request, v10_detail::ofp_aggregate_stats_request
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_AGGREGATE;

        explicit aggregate_stats_request(
                match_set const& match, std::uint32_t const xid = get_xid())
            : aggregate_stats_request{match, 0xff, OFPP_NONE, xid}
        {
        }

        aggregate_stats_request(
                  match_set const& match
                , std::uint8_t const table_id, std::uint16_t const out_port 
                , std::uint32_t const xid = get_xid())
            : stats_request_adaptor{xid}
            , aggregate_stats_{match.ofp_match(), table_id, 0, out_port}
        {
        }

    private:
        friend stats_request_adaptor;

        auto body() const
            -> v10_detail::ofp_aggregate_stats_request const&
        {
            return aggregate_stats_;
        }

        aggregate_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_aggregate_stats_request const& aggregate_stats)
            : stats_request_adaptor{stats_request}
            , aggregate_stats_(aggregate_stats)
        {
        }

    private:
        v10_detail::ofp_aggregate_stats_request aggregate_stats_;
    };


    class aggregate_stats_reply
        : public v10_detail::stats_reply_adaptor<
                aggregate_stats_reply, v10_detail::ofp_aggregate_stats_reply, false
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_AGGREGATE;

        auto packet_count() const
            -> std::uint64_t
        {
            return aggregate_stats_.packet_count;
        }

        auto byte_count() const
            -> std::uint64_t
        {
            return aggregate_stats_.byte_count;
        }

        auto flow_count() const
            -> std::uint32_t
        {
            return aggregate_stats_.flow_count;
        }

    private:
        friend stats_reply_adaptor;

        auto body() const
            -> v10_detail::ofp_aggregate_stats_reply const&
        {
            return aggregate_stats_;
        }

        aggregate_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , v10_detail::ofp_aggregate_stats_reply const& aggregate_stats)
            : stats_reply_adaptor{stats_reply}
            , aggregate_stats_(aggregate_stats)
        {
        }

    private:
        v10_detail::ofp_aggregate_stats_reply aggregate_stats_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_AGGREGATE_STATS_HPP
