#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_QUEUE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_QUEUE_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/stats_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

namespace stats {

    class queue_stats
    {
    public:
        static std::uint16_t const base_size = sizeof(v10_detail::ofp_queue_stats);

        auto port_no() const
            -> std::uint16_t
        {
            return queue_stats_.port_no;
        }

        auto queue_id() const
            -> std::uint32_t
        {
            return queue_stats_.queue_id;
        }

        auto tx_bytes() const
            -> std::uint64_t
        {
            return queue_stats_.tx_bytes;
        }

        auto tx_packets() const
            -> std::uint64_t
        {
            return queue_stats_.tx_packets;
        }

        auto tx_errors() const
            -> std::uint64_t
        {
            return queue_stats_.tx_errors;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, queue_stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> queue_stats
        {
            return queue_stats{
                detail::decode<v10_detail::ofp_queue_stats>(first, last)
            };
        }

    private:
        explicit queue_stats(v10_detail::ofp_queue_stats const& queue_stats)
            : queue_stats_(queue_stats)
        {
        }

    private:
        v10_detail::ofp_queue_stats queue_stats_;
    };

} // namespace stats

namespace messages {

    class queue_stats_request
        : public v10_detail::stats_request_adaptor<
                queue_stats_request, v10_detail::ofp_queue_stats_request
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_QUEUE;

        explicit queue_stats_request(
                  std::uint16_t const port_no = OFPP_ALL
                , std::uint32_t const queue_id = OFPQ_ALL
                , std::uint32_t const xid = get_xid())
            : stats_request_adaptor{xid}
            , queue_stats_{port_no, {0}, queue_id}
        {
        }

    private:
        friend stats_request_adaptor;

        auto body() const
            -> v10_detail::ofp_queue_stats_request const&
        {
            return queue_stats_;
        }

        queue_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_queue_stats_request const& queue_stats)
            : stats_request_adaptor{stats_request}
            , queue_stats_(queue_stats)
        {
        }

    private:
        v10_detail::ofp_queue_stats_request queue_stats_;
    };


    class queue_stats_reply
        : public v10_detail::stats_reply_adaptor<
                queue_stats_reply, stats::queue_stats, true
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_QUEUE;

        using iterator = std::vector<stats::queue_stats>::const_iterator;
        using const_iterator = std::vector<stats::queue_stats>::const_iterator;

        auto num_queues() const
            -> std::size_t
        {
            return queue_stats_.size();
        }

        auto begin() const
            -> const_iterator
        {
            return queue_stats_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return queue_stats_.end();
        }

    private:
        friend stats_reply_adaptor;

        auto body() const
            -> std::vector<stats::queue_stats> const&
        {
            return queue_stats_;
        }

        queue_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , std::vector<stats::queue_stats> queue_stats)
            : stats_reply_adaptor{stats_reply}
            , queue_stats_(std::move(queue_stats))
        {
        }

    private:
        std::vector<stats::queue_stats> queue_stats_;
    };

} // namespace messages

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_QUEUE_STATS_HPP
