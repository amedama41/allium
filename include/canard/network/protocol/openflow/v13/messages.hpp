#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/message/hello.hpp>
#include <canard/network/protocol/openflow/v13/message/error.hpp>
#include <canard/network/protocol/openflow/v13/message/echo.hpp>
#include <canard/network/protocol/openflow/v13/message/flow_mod.hpp>
#include <canard/network/protocol/openflow/v13/message/group_mod.hpp>
#include <canard/network/protocol/openflow/v13/message/switch_features.hpp>
#include <canard/network/protocol/openflow/v13/message/switch_config.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_messages.hpp>
#include <canard/network/protocol/openflow/v13/message/packet_out.hpp>
#include <canard/network/protocol/openflow/v13/message/barrier.hpp>
#include <canard/network/protocol/openflow/v13/message/queue_config.hpp>
#include <canard/network/protocol/openflow/v13/message/packet_in.hpp>
#include <canard/network/protocol/openflow/v13/message/flow_removed.hpp>
#include <canard/network/protocol/openflow/v13/message/port_status.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

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
        // , messages::role_reply
        // , messages::get_async_reply
    >;

    using default_multipart_reply_list = std::tuple<
          messages::multipart::description_reply
        , messages::multipart::flow_stats_reply
        , messages::multipart::aggregate_stats_reply
        , messages::multipart::table_stats_reply
        , messages::multipart::port_stats_reply
        , messages::multipart::queue_stats_reply
        // , messages::group_stats_reply
        // , messages::group_description_reply
        // , messages::gruop_features_reply
        // , messages::meter_stats_reply
        // , messages::meter_config_reply
        // , messages::meter_features_reply
        , messages::multipart::table_features_reply
        , messages::multipart::port_description_reply
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_HPP
