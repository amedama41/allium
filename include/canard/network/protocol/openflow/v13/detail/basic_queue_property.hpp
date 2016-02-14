#ifndef CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_BAISC_QUEUE_PROPERTY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_BAISC_QUEUE_PROPERTY_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace queue_properties {
namespace queue_property_detail {

    template <class T>
    class basic_queue_property
    {
    public:
        static constexpr auto property() noexcept
            -> protocol::ofp_queue_properties
        {
            return T::queue_property;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return static_cast<T const*>(this)->queue_property_header().len;
        }
    };

} // namespace queue_property_detail
} // namespace queue_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_BAISC_QUEUE_PROPERTY_HPP
