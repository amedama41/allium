#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP

#include <tuple>
#include <canard/network/protocol/openflow/v13/instruction/apply_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/clear_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/goto_table.hpp>
#include <canard/network/protocol/openflow/v13/instruction/meter.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_metadata.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using default_instruction_list = std::tuple<
          instructions::goto_table
        , instructions::write_metadata
        , instructions::write_actions
        , instructions::apply_actions
        , instructions::clear_actions
        , instructions::meter
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
