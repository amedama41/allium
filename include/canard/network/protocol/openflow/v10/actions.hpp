#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/action/enqueue.hpp>
#include <canard/network/protocol/openflow/v10/action/output.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_action_list = std::tuple<
          actions::output
        , actions::enqueue
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_HPP
