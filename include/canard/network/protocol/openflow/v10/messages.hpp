#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/message/barrier.hpp>
#include <canard/network/protocol/openflow/v10/message/error_msg.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_mod_add.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_removed.hpp>
#include <canard/network/protocol/openflow/v10/message/packet_in.hpp>
#include <canard/network/protocol/openflow/v10/message/packet_out.hpp>
#include <canard/network/protocol/openflow/v10/message/port_status.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/aggregate_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/description.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/flow_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_config.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_features.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_reply_list = std::tuple<
          messages::get_config_reply
        , messages::features_reply
        , messages::barrier_reply
    >;

    using default_async_list = std::tuple<
          messages::packet_in
        , messages::flow_removed
        , messages::port_status
        , messages::error_msg
    >;

    using default_switch_message_list = std::tuple<
          messages::get_config_reply
        , messages::features_reply
        , messages::barrier_reply
        , messages::packet_in
        , messages::flow_removed
        , messages::port_status
        , messages::error_msg
    >;

    using default_stats_reply_list = std::tuple<
          messages::description_reply
        , messages::flow_stats_reply
        , messages::aggregate_stats_reply
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
