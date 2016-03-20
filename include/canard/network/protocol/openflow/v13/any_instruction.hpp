#ifndef CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP

#include <cstdint>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/detail/any_instruction.hpp>
#include <canard/network/protocol/openflow/v13/decoder/instruction_decoder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using any_instruction = detail::any_instruction<instruction_decoder>;

    template <class T>
    auto any_cast(any_instruction const& instruction)
        -> T const&
    {
        return detail::any_cast<T>(instruction);
    }

    template <class T>
    auto any_cast(any_instruction const* const instruction)
        -> T const*
    {
        return detail::any_cast<T>(instruction);
    }

    namespace any_instruction_detail {

        struct instruction_order_visitor
            : boost::static_visitor<std::uint64_t>
        {
            template <class T>
            auto operator()(T const& t) const
                -> std::uint64_t
            {
                return instruction_order(t);
            }
        };

    } // namespace any_instruction_detail

    inline auto instruction_order(any_instruction const& instruction)
        -> std::uint64_t
    {
        auto visitor = any_instruction_detail::instruction_order_visitor{};
        return instruction.visit(visitor);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP
