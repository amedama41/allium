#ifndef CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP

#include <canard/network/protocol/openflow/detail/any_action.hpp>
#include <canard/network/protocol/openflow/v10/actions.hpp>
#include <canard/network/protocol/openflow/v10/decoder/action_decoder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using any_action = detail::any_action<
        default_action_list, action_decoder, protocol
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ANY_ACTION_HPP
