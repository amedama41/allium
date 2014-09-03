#ifndef CANARD_NETWORK_OPENFLOW_V13_REQEUST_TO_REPLY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_REQEUST_TO_REPLY_HPP

#include <canard/network/protocol/openflow/v13/messages.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    template <class T>
    struct request_to_reply
    {
        using type = error_msg;
    };

    template <>
    struct request_to_reply<echo_request>
    {
        using type = echo_reply;
    };

    template <>
    struct request_to_reply<features_request>
    {
        using type = features_reply;
    };

    template <>
    struct request_to_reply<switch_config_request>
    {
        using type = switch_config_reply;
    };

    template <>
    struct request_to_reply<barrier_request>
    {
        using type = barrier_reply;
    };

    template <>
    struct request_to_reply<description_request>
    {
        using type = description_reply;
    };

    template <>
    struct request_to_reply<flow_stats_request>
    {
        using type = flow_stats_reply;
    };

    template <>
    struct request_to_reply<aggregate_stats_request>
    {
        using type = aggregate_stats_reply;
    };

    template <>
    struct request_to_reply<table_stats_request>
    {
        using type = table_stats_reply;
    };

    template <>
    struct request_to_reply<port_stats_request>
    {
        using type = port_stats_reply;
    };

    template <>
    struct request_to_reply<table_features_request>
    {
        using type = table_features_reply;
    };

    template <>
    struct request_to_reply<port_description_request>
    {
        using type = port_description_reply;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_REQEUST_TO_REPLY_HPP
