#ifndef CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_MIN_RATE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_MIN_RATE_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace queue_properties {

    class min_rate
    {
    public:
        static protocol::ofp_queue_properties const queue_property
            = protocol::OFPQT_MIN_RATE;
        static std::uint16_t const raw_size
            = sizeof(v10_detail::ofp_queue_prop_min_rate);

        auto property() const
            -> protocol::ofp_queue_properties
        {
            return queue_property;
        }

        auto length() const
            -> std::uint16_t
        {
            return raw_size;
        }

        auto rate() const
            -> std::uint16_t
        {
            return min_rate_.rate;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, min_rate_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> min_rate
        {
            return min_rate{
                detail::decode<v10_detail::ofp_queue_prop_min_rate>(first, last)
            };
        }

    private:
        explicit min_rate(v10_detail::ofp_queue_prop_min_rate const& min_rate)
            : min_rate_(min_rate)
        {
            if (min_rate.prop_header.len != raw_size) {
                throw std::runtime_error{"invalid queue property min rate length"};
            }
        }

    private:
        v10_detail::ofp_queue_prop_min_rate min_rate_;
    };

} // namespace queue_properties
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_MIN_RATE_HPP
