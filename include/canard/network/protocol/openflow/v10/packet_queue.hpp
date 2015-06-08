#ifndef CANARD_NETWORK_OPENFLOW_V10_PACKET_QUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_PACKET_QUEUE_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/any_queue_property.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace packet_queue_detail {

        inline auto max_num_of_properties(std::size_t const properties_length)
            -> std::size_t
        {
            return properties_length / any_queue_property::min_raw_size;
        }

    } // namespace packet_queue_detail


    class packet_queue
    {
        using property_container = std::vector<any_queue_property>;

    public:
        using value_type = property_container::value_type;
        using reference = property_container::const_reference;
        using const_reference = property_container::const_reference;
        using iterator = property_container::const_iterator;
        using const_iterator = property_container::const_iterator;
        using difference_type = property_container::difference_type;

        auto queue_id() const
            -> std::uint32_t
        {
            return packet_queue_.queue_id;
        }

        auto length() const
            -> std::uint16_t
        {
            return packet_queue_.len;
        }

        auto begin() const
            -> const_iterator
        {
            return properties_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return properties_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_queue_);
            boost::for_each(properties_, [&](any_queue_property const& prop) {
                prop.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_queue
        {
            auto const pkt_queue
                = detail::decode<v10_detail::ofp_packet_queue>(first, last);
            auto const properties_length = pkt_queue.len - sizeof(pkt_queue);
            if (std::distance(first, last) < properties_length) {
                throw std::runtime_error{"invalid packet_queue length"};
            }
            auto properties = property_container{};
            properties.reserve(
                    packet_queue_detail::max_num_of_properties(properties_length));
            auto const prop_last = std::next(first, properties_length);
            while (first != prop_last) {
                properties.push_back(any_queue_property::decode(first, prop_last));
            }
            return packet_queue{pkt_queue, std::move(properties)};
        }

    private:
        packet_queue(
                  v10_detail::ofp_packet_queue const& pkt_queue
                , property_container properties)
            : packet_queue_(pkt_queue)
            , properties_(std::move(properties))
        {
        }

    private:
        v10_detail::ofp_packet_queue packet_queue_;
        property_container properties_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_PACKET_QUEUE_HPP
