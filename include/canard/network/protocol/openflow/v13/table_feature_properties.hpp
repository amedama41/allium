#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/table_feature_property/actions.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_property/instructions.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_property/next_tables.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_property/oxm.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using default_table_feature_property_list = std::tuple<
          table_feature_properties::instructions
        , table_feature_properties::instructions_miss
        , table_feature_properties::next_tables
        , table_feature_properties::next_tables_miss
        , table_feature_properties::write_actions
        , table_feature_properties::write_actions_miss
        , table_feature_properties::apply_actions
        , table_feature_properties::apply_actions_miss
        , table_feature_properties::match
        , table_feature_properties::wildcards
        , table_feature_properties::write_setfield
        , table_feature_properties::write_setfield_miss
        , table_feature_properties::apply_setfield
        , table_feature_properties::apply_setfield_miss
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_HPP
