#ifndef CANARD_NETWORK_OPENFLOW_V13_TO_ERROR_CODE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TO_ERROR_CODE_HPP

#include <tuple>
#include <type_traits>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/error.hpp>
#include <canard/network/protocol/openflow/v13/message/error.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    auto to_error_code(protocol::ofp_error_type const type, std::uint16_t const value)
        -> boost::system::error_code
    {
        using error_category_list = std::tuple<
              std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_HELLO_FAILED>,          protocol::ofp_hello_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_BAD_REQUEST>,           protocol::ofp_bad_request_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_BAD_ACTION>,            protocol::ofp_bad_action_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_BAD_INSTRUCTION>,       protocol::ofp_bad_instruction_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_BAD_MATCH>,             protocol::ofp_bad_match_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_FLOW_MOD_FAILED>,       protocol::ofp_flow_mod_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_GROUP_MOD_FAILED>,      protocol::ofp_group_mod_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_PORT_MOD_FAILED>,       protocol::ofp_port_mod_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_TABLE_MOD_FAILED>,      protocol::ofp_table_mod_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_QUEUE_OP_FAILED>,       protocol::ofp_queue_op_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_SWITCH_CONFIG_FAILED>,  protocol::ofp_switch_config_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_ROLE_REQUEST_FAILED>,   protocol::ofp_role_request_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_METER_MOD_FAILED>,      protocol::ofp_meter_mod_failed_code>
            , std::tuple<std::integral_constant<protocol::ofp_error_type, protocol::OFPET_TABLE_FEATURES_FAILED>, protocol::ofp_table_features_failed_code>
        >;

        switch (type) {
#       define CANARD_TETWORK_OPENFLOW_V13_ERROR_CATEGORY_CASE(z, N, _) \
        case std::tuple_element<0, std::tuple_element<N, error_category_list>::type>::type::value: \
            return std::tuple_element<1, std::tuple_element<N, error_category_list>::type>::type(value);
        BOOST_PP_REPEAT(14, CANARD_TETWORK_OPENFLOW_V13_ERROR_CATEGORY_CASE, _)
#       undef CANARD_TETWORK_OPENFLOW_V13_ERROR_CATEGORY_CASE
        // TODO
        case protocol::OFPET_EXPERIMENTER:
        default:
            return boost::system::error_code{value, boost::system::generic_category()};
        }
    }

    auto to_error_code(error const& error)
        -> boost::system::error_code
    {
        return to_error_code(error.error_type(), error.error_code());
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TO_ERROR_CODE_HPP
