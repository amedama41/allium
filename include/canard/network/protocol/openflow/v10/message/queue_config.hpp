#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGE_QUEUE_CONFIG_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGE_QUEUE_CONFIG_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/packet_queue.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    namespace queue_config_detail {

        inline auto max_num_of_queues(std::size_t const queues_length)
            -> std::size_t
        {
            return queues_length / sizeof(v10_detail::ofp_packet_queue);
        }

    } // namespace queue_config_detail


    class queue_get_config_request
        : public v10_detail::basic_openflow_message<queue_get_config_request>
    {
    public:
        static ofp_type const message_type = OFPT_QUEUE_GET_CONFIG_REQUEST;

        explicit queue_get_config_request(
                std::uint16_t const port, std::uint32_t const xid = get_xid())
            : queue_get_config_{
                  {OFP_VERSION, message_type, sizeof(queue_get_config_), xid}
                , port, {0}
              }
        {
            if (this->port() > OFPP_MAX) {
                throw std::runtime_error{"invalid port number"};
            }
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return queue_get_config_.header;
        }

        auto port() const
            -> std::uint16_t
        {
            return queue_get_config_.port;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, queue_get_config_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> queue_get_config_request
        {
            return queue_get_config_request{
                detail::decode<v10_detail::ofp_queue_get_config_request>(first, last)
            };
        }

    private:
        explicit queue_get_config_request(
                v10_detail::ofp_queue_get_config_request const& queue_get_config)
            : queue_get_config_(queue_get_config)
        {
        }

    private:
        v10_detail::ofp_queue_get_config_request queue_get_config_;
    };


    class queue_get_config_reply
        : public v10_detail::basic_openflow_message<queue_get_config_reply>
    {
        using queue_container = std::vector<packet_queue>;

    public:
        static ofp_type const message_type = OFPT_QUEUE_GET_CONFIG_REPLY;

        using value_type = queue_container::value_type;
        using reference = queue_container::const_reference;
        using const_reference = queue_container::const_reference;
        using iterator = queue_container::const_iterator;
        using const_iterator = queue_container::const_iterator;
        using difference_type = queue_container::difference_type;

        auto header() const
            -> v10_detail::ofp_header
        {
            return queue_get_config_.header;
        }

        auto port() const
            -> std::uint16_t
        {
            return queue_get_config_.port;
        }

        auto begin() const
            -> const_iterator
        {
            return queues_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return queues_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, queue_get_config_);
            boost::for_each(queues_, [&](packet_queue const& queue) {
                queue.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> queue_get_config_reply
        {
            auto const queue_get_config
                = detail::decode<v10_detail::ofp_queue_get_config_reply>(first, last);
            auto queues = queue_container{};
            queues.reserve(queue_config_detail::max_num_of_queues(
                        queue_get_config.header.length - sizeof(queue_get_config)));
            while (first != last) {
                queues.push_back(packet_queue::decode(first, last));
            }
            return queue_get_config_reply{queue_get_config, std::move(queues)};
        }

    private:
        queue_get_config_reply(
                  v10_detail::ofp_queue_get_config_reply const& queue_get_config
                , queue_container queues)
            : queue_get_config_(queue_get_config)
            , queues_(std::move(queues))
        {
            if (port() > OFPP_MAX) {
                std::runtime_error{"invalid port number"};
            }
        }

    private:
        v10_detail::ofp_queue_get_config_reply queue_get_config_;
        queue_container queues_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGE_QUEUE_CONFIG_HPP
