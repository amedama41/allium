#ifndef CANARD_NETWORK_OPENFLOW_V13_PACKET_QUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PACKET_QUEUE_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/queue_id.hpp>
#include <canard/network/protocol/openflow/v13/any_queue_property.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class packet_queue
    {
        static constexpr std::size_t base_packet_queue_size
            = sizeof(v13_detail::ofp_packet_queue);

    public:
        using properties_type = std::vector<any_queue_property>;
        using iterator = properties_type::const_iterator;
        using const_iterator = properties_type::const_iterator;

        packet_queue(std::uint32_t const queue_id
                   , std::uint32_t const port_no
                   , properties_type properties)
            : packet_queue_{
                  queue_id
                , port_no
                , std::uint16_t(
                          base_packet_queue_size
                        + calc_propertis_length(properties))
                , { 0, 0, 0, 0, 0, 0 }
              }
            , properties_(std::move(properties))
        {
        }

        packet_queue(queue_id const& id, properties_type properties)
            : packet_queue{id.queue(), id.port(), std::move(properties)}
        {
        }

        packet_queue(packet_queue const&) = default;

        packet_queue(packet_queue&& other)
            : packet_queue_(other.packet_queue_)
            , properties_(std::move(other).properties_)
        {
            other.packet_queue_.len = base_packet_queue_size;
        }

        auto operator=(packet_queue const&)
            -> packet_queue& = default;

        auto operator=(packet_queue&& other)
            -> packet_queue&
        {
            auto tmp = std::move(other);
            std::swap(packet_queue_, tmp.packet_queue_);
            properties_.swap(tmp.properties_);
            return *this;
        }

        auto queue_id() const noexcept
            -> std::uint32_t
        {
            return packet_queue_.queue_id;
        }

        auto port_no() const noexcept
            -> std::uint32_t
        {
            return packet_queue_.port;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return packet_queue_.len;
        }

        auto properties() const noexcept
            -> properties_type const&
        {
            return properties_;
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return properties_.begin();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return properties_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_queue_);
            boost::for_each(properties_, [&](any_queue_property const& e) {
                e.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_queue
        {
            auto const pkt_queue
                = detail::decode<v13_detail::ofp_packet_queue>(first, last);
            if (pkt_queue.len < base_packet_queue_size) {
                throw std::runtime_error{"packet_queue length is too small"};
            }
            if (std::distance(first, last)
                    < pkt_queue.len - base_packet_queue_size) {
                throw std::runtime_error{"packet_queue length is too big"};
            }
            auto const properties_length
                = pkt_queue.len - base_packet_queue_size;
            last = std::next(first, properties_length);

            auto properties = properties_type{};
            properties.reserve(
                    properties_length / any_queue_property::min_base_size);
            while (std::distance(first, last)
                    >= sizeof(v13_detail::ofp_queue_prop_header)) {
                properties.push_back(any_queue_property::decode(first, last));
            }
            if (first != last) {
                throw std::runtime_error{"invalid packet_queue length"};
            }

            return packet_queue{pkt_queue, std::move(properties)};
        }

    private:
        packet_queue(v13_detail::ofp_packet_queue const& pkt_queue
                   , properties_type&& properties)
            : packet_queue_(pkt_queue)
            , properties_(std::move(properties))
        {
        }

        static auto calc_propertis_length(properties_type const& properties)
            -> std::size_t
        {
            auto const properties_length = boost::accumulate(
                      properties, std::size_t{0}
                    , [](std::size_t const sum, any_queue_property const& e) {
                            return sum + e.length();
                      });
            if (properties_length + sizeof(v13_detail::ofp_packet_queue)
                    > std::numeric_limits<std::uint16_t>::max()) {
                throw std::runtime_error{"properties length is too big"};
            }
            return properties_length;
        }

    private:
        v13_detail::ofp_packet_queue packet_queue_;
        properties_type properties_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PACKET_QUEUE_HPP
