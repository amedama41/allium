#ifndef CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_MIN_RATE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_MIN_RATE_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_queue_property.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace queue_properties {

    class min_rate
        : public queue_property_detail::basic_queue_property<min_rate>
    {
    public:
        static constexpr protocol::ofp_queue_properties queue_property
            = protocol::OFPQT_MIN_RATE;

        static constexpr std::size_t base_size
            = sizeof(v13_detail::ofp_queue_prop_min_rate);

        explicit min_rate(std::uint16_t const rate) noexcept
            : min_rate_{
                  v13_detail::ofp_queue_prop_header{
                      queue_property
                    , sizeof(v13_detail::ofp_queue_prop_min_rate)
                    , { 0, 0, 0, 0 }
                  }
                , rate
                , { 0, 0, 0, 0, 0, 0 }
              }
        {
        }

        auto queue_property_header() const noexcept
            -> v13_detail::ofp_queue_prop_header const&
        {
            return min_rate_.prop_header;
        }

        auto rate() const noexcept
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
                detail::decode<v13_detail::ofp_queue_prop_min_rate>(first, last)
            };
        }

        static void validate(
                v13_detail::ofp_queue_prop_header const& prop_header)
        {
            if (prop_header.property != queue_property) {
                throw std::runtime_error{"invalid queue property"};
            }
            if (prop_header.len != base_size) {
                throw std::runtime_error{"invalid queue property length"};
            }
        }

    private:
        explicit min_rate(
                v13_detail::ofp_queue_prop_min_rate const& min_rate) noexcept
            : min_rate_(min_rate)
        {
        }

    private:
        v13_detail::ofp_queue_prop_min_rate min_rate_;
    };

} // namespace queue_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_MIN_RATE_HPP
