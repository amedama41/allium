#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_INSTRUCTION_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_INSTRUCTION_ACTIONS_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

    template <class T>
    class basic_instruction_actions
        : private boost::equality_comparable<T>
    {
    protected:
        using raw_ofp_type = openflow::v13::v13_detail::ofp_instruction_actions;

    public:
        static constexpr auto type() noexcept
            -> openflow::v13::protocol::ofp_instruction_type
        {
            return T::instruction_type;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return instruction_actions_.len;
        }

        auto actions() const noexcept
            -> openflow::v13::action_list const&
        {
            return actions_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, instruction_actions_);
            return actions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const instruction_actions
                = detail::decode<raw_ofp_type>(first, last);
            last = std::next(
                    first, instruction_actions.len - sizeof(raw_ofp_type));
            auto actions = openflow::v13::action_list::decode(first, last);
            return T{instruction_actions, std::move(actions)};
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> T
        {
            return T::validate(T(std::forward<Args>(args)...));
        }

        static void validate_instruction(
                openflow::v13::v13_detail::ofp_instruction const& instruction)
        {
            if (instruction.type != T::instruction_type) {
                throw std::runtime_error{"invalid instruction type"};
            }
            if (instruction.len < sizeof(raw_ofp_type)) {
                throw std::runtime_error{"instruction length is too small"};
            }
        }

        template <class Instruction>
        static auto validate(Instruction&& instruction)
            -> typename std::enable_if<
                  canard::is_same_value_type<Instruction, T>::value
                , Instruction&&
               >::type
        {
            T::validate_impl(instruction);
            return std::forward<Instruction>(instruction);
        }

        friend auto operator==(T const& lhs, T const& rhs)
            -> bool
        {
            return lhs.actions() == rhs.actions();
        }

    protected:
        explicit basic_instruction_actions(openflow::v13::action_list&& actions)
            : instruction_actions_{
                  T::instruction_type
                , std::uint16_t(sizeof(raw_ofp_type) + actions.length())
                , { 0, 0, 0, 0 }
              }
            , actions_(std::move(actions))
        {
        }

        basic_instruction_actions(
                  raw_ofp_type const& instruction_actions
                , openflow::v13::action_list&& actions)
            : instruction_actions_(instruction_actions)
            , actions_(std::move(actions))
        {
        }

        basic_instruction_actions(basic_instruction_actions const&) = default;

        basic_instruction_actions(basic_instruction_actions&& other)
            : instruction_actions_(other.instruction_actions_)
            , actions_(std::move(other).actions_)
        {
            other.instruction_actions_.len = sizeof(raw_ofp_type);
        }

        auto operator=(basic_instruction_actions const&)
            -> basic_instruction_actions& = default;

        auto operator=(basic_instruction_actions&& other)
            -> basic_instruction_actions&
        {
            auto tmp = std::move(other);
            std::swap(instruction_actions_, tmp.instruction_actions_);
            actions_.swap(tmp.actions_);
            return *this;
        }

    private:
        raw_ofp_type instruction_actions_;
        openflow::v13::action_list actions_;
    };

} // namespace v13
} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_INSTRUCTION_ACTIONS_HPP
