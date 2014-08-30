#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>
#include <boost/preprocessor/repeat.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
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

    template <class ReturnType, class Iterator, class Function>
    auto decode_instruction(Iterator& first, Iterator last, Function func)
        -> ReturnType
    {
        auto instruction = detail::ofp_instruction{};
        std::copy_n(first, sizeof(instruction), canard::as_byte_range(instruction).begin());

        using instruction_list = std::tuple<
              instructions::goto_table
            , instructions::write_metadata
            , instructions::write_actions
            , instructions::apply_actions
            , instructions::clear_actions
            , instructions::meter
        >;

        switch (detail::ntoh(instruction.type)) {
#       define CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE(z, N, _) \
        case std::tuple_element<N, instruction_list>::type::instruction_type: \
            return func(std::tuple_element<N, instruction_list>::type::decode(first, last));
        BOOST_PP_REPEAT(6, CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE

        default:
            std::advance(first, detail::ntoh(instruction.len));
            throw 3;
            // TODO return func(any_instruction{});
        }
    }

    namespace detail {

        struct to_any_instruction
        {
            template <class Instruction>
            auto operator()(Instruction&& instruction) const
                -> any_action
            {
                return any_instruction{std::forward<Instruction>(instruction)};
            }
        };

    } // namespace detail

    template <class Iterator>
    auto decode_instruction(Iterator& first, Iterator last)
        -> any_instruction
    {
        return decode_instruction<any_instruction>(first, last, detail::to_any_instruction{});
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_HPP
