#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_QUEUE_PROPERTY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_QUEUE_PROPERTY_HPP

#include <canard/network/protocol/openflow/detail/any_queue_property.hpp>
#include <canard/network/protocol/openflow/v13/decoder/queue_property_decoder.hpp>
#include <canard/network/protocol/openflow/v13/queue_properties.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using any_queue_property = detail::any_queue_property<
        default_queue_property_list, queue_property_decoder
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_QUEUE_PROPERTY_HPP
