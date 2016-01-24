#ifndef CANARD_NETWORK_OPENFLOW_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_ERROR_HPP

#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/message/error.hpp>

namespace canard {
namespace network {
namespace openflow {

    using error = v13::messages::error;

    constexpr auto OFPET_HELLO_FAILED = v13::protocol::OFPET_HELLO_FAILED;

    constexpr auto OFPHFC_INCOMPATIBLE = v13::protocol::OFPHFC_INCOMPATIBLE;
    constexpr auto OFPHFC_EPERM = v13::protocol::OFPHFC_EPERM;

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ERROR_HPP
