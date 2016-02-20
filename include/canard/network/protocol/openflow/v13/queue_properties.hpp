#ifndef CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/queue_property/max_rate.hpp>
#include <canard/network/protocol/openflow/v13/queue_property/min_rate.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using default_queue_property_list = std::tuple<
          queue_properties::min_rate
        , queue_properties::max_rate
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTIES_HPP
