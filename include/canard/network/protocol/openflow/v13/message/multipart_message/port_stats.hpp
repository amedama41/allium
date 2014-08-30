#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_STATS_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class port_stats
    {
    public:
        auto port_no() const
            -> std::uint32_t
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

        auto rx_frame_err() const
            -> std::uint64_t
        {
            return port_stats_.rx_frame_err;
        }

        auto rx_over_err() const
            -> std::uint64_t
        {
            return port_stats_.rx_over_err;
        }

        auto rx_crc_err() const
            -> std::uint64_t
        {
            return port_stats_.rx_crc_err;
        }

        auto collisions() const
            -> std::uint64_t
        {
            return port_stats_.collisions;
        }

        auto duration_sec() const
            -> std::uint32_t
        {
            return port_stats_.duration_sec;
        }

        auto duration_nsec() const
            -> std::uint32_t
        {
            return port_stats_.duration_nsec;
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
            return port_stats{detail::decode<detail::ofp_port_stats>(first, last)};
        }

    private:
        explicit port_stats(detail::ofp_port_stats const& stats)
            : port_stats_(stats)
        {
        }

    private:
        detail::ofp_port_stats port_stats_;
    };

    class port_stats_request
        : public detail::basic_multipart_request<port_stats_request>
    {
    public:
        static ofp_multipart_type const multipart_type_value = OFPMP_PORT_STATS;

        port_stats_request()
            : port_stats_request{OFPP_ANY}
        {
        }

        explicit port_stats_request(std::uint32_t const port_no)
            : basic_multipart_request{sizeof(detail::ofp_port_stats_request), 0}
            , port_stats_request_{port_no, {0, 0, 0, 0}}
        {
        }

        auto port_no() const
            -> std::uint32_t
        {
            return port_stats_request_.port_no;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_request::encode(container);
            return detail::encode(container, port_stats_request_);
        }

    private:
        detail::ofp_port_stats_request port_stats_request_;
    };


    class port_stats_reply
        : public detail::basic_multipart_reply<port_stats_reply>
    {
        using port_stats_list = std::vector<port_stats>;

    public:
        static ofp_multipart_type const multipart_type_value = OFPMP_PORT_STATS;

        using iterator = port_stats_list::iterator;
        using const_iterator = port_stats_list::const_iterator;

        auto begin() const
            -> const_iterator
        {
            return port_stats_list_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return port_stats_list_.end();
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_stats_reply
        {
            auto const reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(detail::ofp_multipart_reply)) {
                throw 2;
            }

            auto stats_list = port_stats_list{};
            stats_list.reserve(std::distance(first, last) / sizeof(detail::ofp_port_stats));
            while (first != last) {
                stats_list.push_back(port_stats::decode(first, last));
            }
            return port_stats_reply{reply, std::move(stats_list)};
        }

    private:
        port_stats_reply(detail::ofp_multipart_reply const& reply, port_stats_list stats_list)
            : basic_multipart_reply{reply}
            , port_stats_list_(std::move(stats_list))
        {
        }

    private:
        port_stats_list port_stats_list_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_STATS_HPP
