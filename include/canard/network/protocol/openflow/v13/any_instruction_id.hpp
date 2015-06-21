#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_INSTRUCTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_INSTRUCTION_ID_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/instruction_id.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_instruction_id
    {
        using instruction_id_variant
            = boost::variant<instruction_id, instruction_experimenter_id>;
    public:
        template <
              class InstructionID
            , typename std::enable_if<!is_related<any_instruction_id, InstructionID>::value>::type* = nullptr
        >
        any_instruction_id(InstructionID&& instruction_id)
            : variant_(std::forward<InstructionID>(instruction_id))
        {
        }

        template <
              class InstructionID
            , typename std::enable_if<!is_related<any_instruction_id, InstructionID>::value>::type* = nullptr
        >
        auto operator=(InstructionID&& instruction_id)
            -> any_instruction_id&
        {
            variant_ = std::forward<InstructionID>(instruction_id);
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
            -> any_instruction_id
        {
            auto copy_first = first;
            auto const type = detail::decode<std::uint16_t>(copy_first, last);
            switch (type) {
            case protocol::OFPIT_EXPERIMENTER:
                return instruction_experimenter_id::decode(first, last);
            default:
                return instruction_id::decode(first, last);
            }
        }

        template <class T>
        friend auto any_cast(any_instruction_id const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_instruction_id const*)
            -> T const*;

    private:
        instruction_id_variant variant_;
    };

    template <class T>
    auto any_cast(any_instruction_id const& instruction_id)
        -> T const&
    {
        return boost::get<T>(instruction_id.variant_);
    }

    template <class T>
    auto any_cast(any_instruction_id const* instruction_id)
        -> T const*
    {
        return boost::get<T>(&instruction_id->variant_);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_INSTRUCTION_ID_HPP
