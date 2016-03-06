#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP

#include <canard/network/protocol/openflow/detail/any_action.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/decoder/action_decoder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using any_action = detail::any_action<
        default_all_action_list, action_decoder, protocol
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_ACTION_HPP
