#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v10/message/barrier.hpp>
#include <canard/network/protocol/openflow/v10/message/echo.hpp>
#include <canard/network/protocol/openflow/v10/message/error.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_add.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_modify.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_delete.hpp>
#include <canard/network/protocol/openflow/v10/message/flow_removed.hpp>
#include <canard/network/protocol/openflow/v10/message/packet_in.hpp>
#include <canard/network/protocol/openflow/v10/message/packet_out.hpp>
#include <canard/network/protocol/openflow/v10/message/port_mod.hpp>
#include <canard/network/protocol/openflow/v10/message/port_status.hpp>
#include <canard/network/protocol/openflow/v10/message/queue_config.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/aggregate_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/description.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/flow_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/port_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/queue_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/table_stats.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_config.hpp>
#include <canard/network/protocol/openflow/v10/message/switch_features.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    using default_reply_list = std::tuple<
          messages::echo_reply
        , messages::features_reply
        , messages::get_config_reply
        , messages::barrier_reply
        , messages::queue_get_config_reply
    >;

    using default_async_list = std::tuple<
          messages::error
        , messages::packet_in
        , messages::flow_removed
        , messages::port_status
    >;

    using default_switch_message_list = std::tuple<
          messages::error
        , messages::echo_request
        , messages::echo_reply
        , messages::features_reply
        , messages::get_config_reply
        , messages::packet_in
        , messages::flow_removed
        , messages::port_status
        , messages::barrier_reply
        , messages::queue_get_config_reply
    >;

    using default_stats_reply_list = std::tuple<
          messages::description_reply
        , messages::statistics::flow_stats_reply
        , messages::aggregate_stats_reply
        , messages::table_stats_reply
        , messages::port_stats_reply
        , messages::queue_stats_reply
    >;

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_HPP
