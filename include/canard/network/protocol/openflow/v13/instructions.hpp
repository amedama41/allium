#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP

#include <cstdint>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/instruction/clear_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/goto_table.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_metadata.hpp>
#include <canard/network/protocol/openflow/v13/instruction/apply_actions.hpp>
#include <canard/network/protocol/openflow/v13/instruction/meter.hpp>

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

    namespace instructions {

        inline constexpr auto instruction_order(meter const&)
            -> std::uint64_t
        {
            return 0x0001000000000000;
        }

        inline constexpr auto instruction_order(apply_actions const&)
            -> std::uint64_t
        {
            return 0x0002000000000000;
        }

        inline constexpr auto instruction_order(clear_actions const&)
            -> std::uint64_t
        {
            return 0x0003000000000000;
        }

        inline constexpr auto instruction_order(write_actions const&)
            -> std::uint64_t
        {
            return 0x0004000000000000;
        }

        inline constexpr auto instruction_order(write_metadata const&)
            -> std::uint64_t
        {
            return 0x0005000000000000;
        }

        inline constexpr auto instruction_order(goto_table const&)
            -> std::uint64_t
        {
            return 0x0006000000000000;
        }

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
