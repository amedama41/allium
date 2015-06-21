#ifndef CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode_instruction.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_instruction
    {
        using instruction_variant = boost::make_variant_over<
            default_instruction_list
        >::type;

    public:
        template <class Instruction, typename std::enable_if<!is_related<any_instruction, Instruction>::value>::type* = nullptr>
        any_instruction(Instruction&& instruction)
            : variant_(std::forward<Instruction>(instruction))
        {
        }

        template <class Instruction, typename std::enable_if<!is_related<any_instruction, Instruction>::value>::type* = nullptr>
        auto operator=(Instruction&& instruction)
            -> any_instruction&
        {
            variant_ = std::forward<Instruction>(instruction);
            return *this;
        }

        auto type() const
            -> protocol::ofp_instruction_type
        {
            auto visitor = detail::type_visitor<protocol::ofp_instruction_type>{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto length() const
            -> std::uint16_t
        {
            auto visitor = detail::length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto visitor = detail::encoding_visitor<Container>{container};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> any_instruction
        {
            return v13_detail::decode_instruction<any_instruction>(first, last, to_any_instruction{});
        }

        friend auto instruction_order(any_instruction const& instruction)
            -> std::uint64_t
        {
            auto visitor = detail::instruction_order_visitor{};
            return boost::apply_visitor(visitor, instruction.variant_);
        }

    private:
        struct to_any_instruction
        {
            template <class Instruction>
            auto operator()(Instruction&& instruction) const
                -> any_action
            {
                return any_instruction{std::forward<Instruction>(instruction)};
            }
        };

    private:
        instruction_variant variant_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ORDERED_INSTRUCTION_HPP
