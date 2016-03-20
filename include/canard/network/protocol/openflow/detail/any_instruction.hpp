#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_ANY_INSTRUCTION_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_ANY_INSTRUCTION_HPP

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/network/protocol/openflow/detail/construct.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    template <class InstructionDecoder>
    class any_instruction
        : private boost::equality_comparable<
            any_instruction<InstructionDecoder>
          >
    {
        using ofp_instruction_type
            = typename InstructionDecoder::ofp_instruction_type;
        using instruction_type_list
            = typename InstructionDecoder::instruction_type_list;
        using instruction_variant
            = typename boost::make_variant_over<instruction_type_list>::type;

    public:
        template <
              class Instruction
            , typename std::enable_if<
                !canard::is_related<any_instruction, Instruction>::value
              >::type* = nullptr
        >
        any_instruction(Instruction&& instruction)
            : variant_(std::forward<Instruction>(instruction))
        {
        }

        template <
              class Instruction
            , typename std::enable_if<
                !canard::is_related<any_instruction, Instruction>::value
              >::type* = nullptr
        >
        auto operator=(Instruction&& instruction)
            -> any_instruction&
        {
            variant_ = std::forward<Instruction>(instruction);
            return *this;
        }

        auto type() const noexcept
            -> ofp_instruction_type
        {
            auto visitor = detail::type_visitor<ofp_instruction_type>{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto length() const noexcept
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
            return InstructionDecoder::template decode<any_instruction>(
                    first, last, construct<any_instruction>{});
        }

        template <class Visitor>
        auto visit(Visitor&& visitor)
            -> typename canard::remove_cvref_t<Visitor>::result_type
        {
            return boost::apply_visitor(
                    std::forward<Visitor>(visitor), variant_);
        }

        template <class Visitor>
        auto visit(Visitor&& visitor) const
            -> typename canard::remove_cvref_t<Visitor>::result_type
        {
            return boost::apply_visitor(
                    std::forward<Visitor>(visitor), variant_);
        }

        friend auto operator==(
                any_instruction const& lhs, any_instruction const& rhs)
            -> bool
        {
            return lhs.variant_ == rhs.variant_;
        }

        template <class T, class Decoder>
        friend auto any_cast(any_instruction<Decoder> const&)
            -> T const&;

        template <class T, class Decoder>
        friend auto any_cast(any_instruction<Decoder> const*)
            -> T const*;

    private:
        instruction_variant variant_;
    };

    template <class T, class InstructionDecoder>
    auto any_cast(any_instruction<InstructionDecoder> const& instruction)
        -> T const&
    {
        return boost::get<T>(instruction.variant_);
    }

    template <class T, class InstructionDecoder>
    auto any_cast(any_instruction<InstructionDecoder> const* const instruction)
        -> T const*
    {
        return boost::get<T>(std::addressof(instruction->variant_));
    }

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_ANY_INSTRUCTION_HPP
