#ifndef CANARD_NETWORK_OPENFLOW_V13_AGGREGATE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_AGGREGATE_STATS_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class aggregate_stats_request
        : public v13_detail::basic_multipart_request<aggregate_stats_request>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_AGGREGATE;

        explicit aggregate_stats_request(oxm_match match)
            : basic_multipart_request{
                detail::exact_length(sizeof(v13_detail::ofp_aggregate_stats_request) + match.length())
                , 0
              }
            , aggregate_stats_request_{
                  protocol::OFPTT_ALL, {0, 0, 0}
                , protocol::OFPP_ANY, protocol::OFPG_ANY
                , {0, 0, 0, 0}, 0, 0
              }
            , match_(std::move(match))
        {
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_request::encode(container);
            detail::encode(container, aggregate_stats_request_);
            return match_.encode(container);
        }

    private:
        v13_detail::ofp_aggregate_stats_request aggregate_stats_request_;
        oxm_match match_;
    };


    class aggregate_stats_reply
        : public v13_detail::basic_multipart_reply<aggregate_stats_reply>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_AGGREGATE;

        aggregate_stats_reply(v13::counters const& counters, std::uint32_t const flow_count)
            : basic_multipart_reply{std::uint16_t(sizeof(v13_detail::ofp_aggregate_stats_reply)), 0}
            , aggregate_stats_reply_{counters.packet_count_, counters.byte_count_, flow_count}
        {
        }

        auto counters() const
            -> v13::counters
        {
            return {packet_count(), byte_count()};
        }

        auto packet_count() const
            -> std::uint64_t
        {
            return aggregate_stats_reply_.packet_count;
        }

        auto byte_count() const
            -> std::uint64_t
        {
            return aggregate_stats_reply_.byte_count;
        }

        auto flow_count() const
            -> std::uint32_t
        {
            return aggregate_stats_reply_.flow_count;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> aggregate_stats_reply
        {
            auto const reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(v13_detail::ofp_multipart_reply)) {
                throw 2;
            }
            auto const stats_reply = detail::decode<v13_detail::ofp_aggregate_stats_reply>(first, last);
            return {reply, stats_reply};
        }

    private:
        aggregate_stats_reply(v13_detail::ofp_multipart_reply const& reply, v13_detail::ofp_aggregate_stats_reply const& stats_reply)
            : basic_multipart_reply{reply}
            , aggregate_stats_reply_(stats_reply)
        {
        }

    private:
        v13_detail::ofp_aggregate_stats_reply aggregate_stats_reply_;
    };

} // namespace messages

using messages::aggregate_stats_request;
using messages::aggregate_stats_reply;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_AGGREGATE_STATS_HPP
