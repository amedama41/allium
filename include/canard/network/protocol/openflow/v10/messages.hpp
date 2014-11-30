#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/message/error_msg.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_mod_add.hpp>
#include <canard/network/protocol/openflow/v10/message/packet_in.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_config.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_features.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_reply_list = std::tuple<
          messages::get_config_reply
        , messages::features_reply
    >;

    using default_async_list = std::tuple<
          messages::packet_in
        , messages::error_msg
    >;

    using default_switch_message_list = std::tuple<
          messages::get_config_reply
        , messages::features_reply
        , messages::packet_in
        , messages::error_msg
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
