#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATS_HPP

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

    class port_stats
    {
    public:
        static std::uint16_t const base_size = sizeof(v10_detail::ofp_port_stats);

        auto port_no() const
            -> std::uint16_t
        {
            return port_stats_.port_no;
        }

        auto rx_packets() const
            -> std::uint64_t
        {
            return port_stats_.rx_packets;
        }

        auto tx_packets() const
            -> std::uint64_t
        {
            return port_stats_.tx_packets;
        }

        auto rx_bytes() const
            -> std::uint64_t
        {
            return port_stats_.rx_bytes;
        }

        auto tx_bytes() const
            -> std::uint64_t
        {
            return port_stats_.tx_bytes;
        }

        auto rx_dropped() const
            -> std::uint64_t
        {
            return port_stats_.rx_dropped;
        }

        auto tx_dropped() const
            -> std::uint64_t
        {
            return port_stats_.tx_dropped;
        }

        auto rx_errors() const
            -> std::uint64_t
        {
            return port_stats_.rx_errors;
        }

        auto tx_errors() const
            -> std::uint64_t
        {
            return port_stats_.tx_errors;
        }

        auto rx_frame_errors() const
            -> std::uint64_t
        {
            return port_stats_.rx_frame_err;
        }

        auto rx_over_errors() const
            -> std::uint64_t
        {
            return port_stats_.rx_over_err;
        }

        auto rx_crc_errors() const
            -> std::uint64_t
        {
            return port_stats_.rx_crc_err;
        }

        auto collisions() const
            -> std::uint64_t
        {
            return port_stats_.collisions;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, port_stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_stats
        {
            return port_stats{
                detail::decode<v10_detail::ofp_port_stats>(first, last)
            };
        }

    private:
        explicit port_stats(v10_detail::ofp_port_stats const& port_stats)
            : port_stats_(port_stats)
        {
        }

    private:
        v10_detail::ofp_port_stats port_stats_;
    };

} // namespace stats


namespace messages {

    class port_stats_request
        : public v10_detail::stats_request_adaptor<
                port_stats_request, v10_detail::ofp_port_stats_request
          >
    {
    public:
        static protocol::ofp_stats_types const stats_type_value
            = protocol::OFPST_PORT;

        explicit port_stats_request(
                  std::uint16_t const port_no = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid())
            : stats_request_adaptor{xid}
            , port_stats_{port_no, {0}}
        {
        }

    private:
        friend stats_request_adaptor;

        auto body() const
            -> v10_detail::ofp_port_stats_request const&
        {
            return port_stats_;
        }

        port_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_port_stats_request const& port_stats)
            : stats_request_adaptor{stats_request}
            , port_stats_(port_stats)
        {
        }

    private:
        v10_detail::ofp_port_stats_request port_stats_;
    };


    class port_stats_reply
        : public v10_detail::stats_reply_adaptor<
                port_stats_reply, stats::port_stats, true
          >
    {
    public:
        static protocol::ofp_stats_types const stats_type_value
            = protocol::OFPST_PORT;

        using iterator = std::vector<stats::port_stats>::const_iterator;
        using const_iterator = std::vector<stats::port_stats>::const_iterator;

        auto num_ports() const
            -> std::uint16_t
        {
            return port_stats_.size();
        }

        auto begin() const
            -> const_iterator
        {
            return port_stats_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return port_stats_.end();
        }

    private:
        friend stats_reply_adaptor;

        auto body() const
            -> std::vector<stats::port_stats> const&
        {
            return port_stats_;
        }

        port_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , std::vector<stats::port_stats> port_stats)
            : stats_reply_adaptor{stats_reply}
            , port_stats_(std::move(port_stats))
        {
        }

    private:
        std::vector<stats::port_stats> port_stats_;
    };

} // namespace messages

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATS_HPP
