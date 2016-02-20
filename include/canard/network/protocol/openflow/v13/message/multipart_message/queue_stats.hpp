#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_QUEUE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_QUEUE_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {

    class queue_stats
    {
        using raw_ofp_type = v13_detail::ofp_queue_stats;

    public:
        static constexpr std::size_t base_size = sizeof(raw_ofp_type);

        queue_stats(std::uint32_t const queue_id
                  , std::uint32_t const port_no
                  , std::uint64_t const tx_packets
                  , std::uint64_t const tx_bytes
                  , std::uint64_t const tx_errors
                  , v13::elapsed_time const& elapsed_time) noexcept
            : queue_stats_{
                  port_no
                , queue_id
                , tx_bytes
                , tx_packets
                , tx_errors
                , elapsed_time.duration_sec()
                , elapsed_time.duration_nsec()
              }
        {
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return base_size;
        }

        auto queue_id() const noexcept
            -> std::uint32_t
        {
            return queue_stats_.queue_id;
        }

        auto port_no() const noexcept
            -> std::uint32_t
        {
            return queue_stats_.port_no;
        }

        auto tx_packets() const noexcept
            -> std::uint64_t
        {
            return queue_stats_.tx_packets;
        }

        auto tx_bytes() const noexcept
            -> std::uint64_t
        {
            return queue_stats_.tx_bytes;
        }

        auto tx_errors() const noexcept
            -> std::uint64_t
        {
            return queue_stats_.tx_errors;
        }

        auto duration_sec() const noexcept
            -> std::uint32_t
        {
            return queue_stats_.duration_sec;
        }

        auto duration_nsec() const noexcept
            -> std::uint32_t
        {
            return queue_stats_.duration_nsec;
        }

        auto elapsed_time() const noexcept
            -> v13::elapsed_time
        {
            return v13::elapsed_time{duration_sec(), duration_nsec()};
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
            return queue_stats{detail::decode<raw_ofp_type>(first, last)};
        }

    private:
        explicit queue_stats(raw_ofp_type const& queue_stats) noexcept
            : queue_stats_(queue_stats)
        {
        }

    private:
        raw_ofp_type queue_stats_;
    };


    class queue_stats_request
        : public multipart_detail::basic_multipart_request<
                queue_stats_request, v13_detail::ofp_queue_stats_request
          >
    {
        using raw_ofp_type = v13_detail::ofp_queue_stats_request;

    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_QUEUE;

        queue_stats_request(
                  std::uint32_t const queue_id
                , std::uint32_t const port_no
                , std::uint32_t const xid = get_xid()) noexcept
            : basic_multipart_request{
                  0
                , raw_ofp_type{port_no, queue_id}
                , xid
              }
        {
        }

        auto queue_id() const noexcept
            -> std::uint32_t
        {
            return body().queue_id;
        }

        auto port_no() const noexcept
            -> std::uint32_t
        {
            return body().port_no;
        }

    private:
        friend basic_multipart_request::base_type;

        queue_stats_request(
                  v13_detail::ofp_multipart_request const& multipart_request
                , raw_ofp_type const& queue_stats_request) noexcept
            : basic_multipart_request{multipart_request, queue_stats_request}
        {
        }
    };


    class queue_stats_reply
        : public multipart_detail::basic_multipart_reply<
          queue_stats_reply, queue_stats[]
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_QUEUE;

        queue_stats_reply(
                  body_type queue_stats
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_multipart_reply{flags, std::move(queue_stats), xid}
        {
        }

    private:
        friend basic_multipart_reply::base_type;

        queue_stats_reply(
                  v13_detail::ofp_multipart_reply const multipart_reply
                , body_type&& queue_stats)
            : basic_multipart_reply{multipart_reply, std::move(queue_stats)}
        {
        }
    };

} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_QUEUE_STATS_HPP
