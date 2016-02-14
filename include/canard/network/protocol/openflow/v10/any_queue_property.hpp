#ifndef CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP

#include <canard/network/protocol/openflow/detail/any_queue_property.hpp>
#include <canard/network/protocol/openflow/v10/decorder/queue_property_decoder.hpp>
#include <canard/network/protocol/openflow/v10/queue_properties.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using any_queue_property = detail::any_queue_property<
        default_queue_property_list, queue_property_decoder
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP
