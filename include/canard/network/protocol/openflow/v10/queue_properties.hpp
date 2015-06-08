#ifndef CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_HPP
#define CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/queue_property/min_rate.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_queue_property_list = std::tuple<
        queue_properties::min_rate
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTIES_HPP
