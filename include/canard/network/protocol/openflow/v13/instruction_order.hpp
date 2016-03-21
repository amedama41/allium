#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ORDER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ORDER_HPP

#include <cstdint>
#include <type_traits>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/support/pair.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instruction_order_detail {

        using boost::fusion::pair;

        template <std::uint16_t Priority>
        using priority = std::integral_constant<std::uint16_t, Priority>;

        using instruction_priority_map = boost::fusion::map<
              pair<instructions::meter, priority<0x0100>>
            , pair<instructions::apply_actions, priority<0x0200>>
            , pair<instructions::clear_actions, priority<0x0200>>
            , pair<instructions::write_actions, priority<0x0300>>
            , pair<instructions::write_metadata, priority<0x0400>>
            , pair<instructions::goto_table, priority<0x0500>>
        >;

        template <class Instruction>
        using has_priority_t = typename boost::fusion::result_of::has_key<
            instruction_order_detail::instruction_priority_map, Instruction
        >::type;

        template <class Instruction>
        using priority_t = typename boost::fusion::result_of::value_at_key<
            instruction_order_detail::instruction_priority_map, Instruction
        >::type;

    } // namespace instruction_order_detail


    template <class Instruction, class = void>
    struct instruction_priority;

    template <class Instruction>
    struct instruction_priority<
        Instruction, typename std::enable_if<
            instruction_order_detail::has_priority_t<Instruction>::value
        >::type
    >
    {
        static constexpr std::uint16_t value
            = instruction_order_detail::priority_t<Instruction>::value;
    };


    template <class Instruction, class = void>
    struct instruction_order;

    template <class Instruction>
    struct instruction_order<
        Instruction, typename std::enable_if<
            (instruction_priority<Instruction>{}, true)
        >::type
    >
    {
        using instruction = Instruction;

        static constexpr std::uint16_t priority
            = instruction_priority<instruction>::value;

        static constexpr std::uint64_t value
            = (std::uint64_t(priority) << 48)
            | (std::uint64_t(instruction::type()) << 32);

        static constexpr auto get_value(instruction const&) noexcept
            -> std::uint64_t
        {
            return value;
        }
    };


    template <class Instruction>
    auto get_order(Instruction const& instruction)
        -> decltype(instruction_order<Instruction>::get_value(instruction))
    {
        return instruction_order<Instruction>::get_value(instruction);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ORDER_HPP
