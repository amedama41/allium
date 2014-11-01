#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP

#include <cstdint>
#include <map>
#include <type_traits>
#include <utility>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/detail/add_helper.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

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

    class instruction_set
    {
    public:
        instruction_set()
            : instruction_map_{}
        {
        }

        template <class... Instructions, typename std::enable_if<!is_related<instruction_set, Instructions...>::value>::type* = nullptr>
        instruction_set(Instructions&&... instructions)
            : instruction_map_{}
        {
            add_impl(std::forward<Instructions>(instructions)...);
        }

        instruction_set(instruction_set const&) = default;
        instruction_set(instruction_set&&) noexcept = default;
        auto operator=(instruction_set&&) noexcept
            -> instruction_set& = default;

        auto operator=(instruction_set const& other)
            -> instruction_set&
        {
            auto tmp = other;
            swap(tmp);
            return *this;
        }

        void swap(instruction_set& other)
        {
            instruction_map_.swap(other.instruction_map_);
        }

        template <class Instruction>
        void add(Instruction&& instruction)
        {
            auto const order = instruction_order(instruction);
            auto const it = instruction_map_.lower_bound(order);
            if (it != instruction_map_.end() && !instruction_map_.key_comp()(order, it->first)) {
                it->second = std::forward<Instruction>(instruction);
            }
            else {
                instruction_map_.emplace_hint(it, order, std::forward<Instruction>(instruction));
            }
        }

        auto length() const
            -> std::uint16_t
        {
            using boost::adaptors::transformed;
            using boost::adaptors::map_values;
            return boost::accumulate(
                        instruction_map_ | map_values | transformed([](any_instruction const& x) { return x.length(); })
                      , std::uint16_t{0});
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            using boost::adaptors::map_values;
            boost::for_each(instruction_map_ | map_values, [&](any_instruction const& instruction) {
                instruction.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_set
        {
            auto insts_set = instruction_set{};
            while (first != last) {
                decode_instruction<void>(first, last, detail::add_helper<instruction_set>{insts_set});
            }
            return insts_set;
        }

    private:
        void add_impl()
        {
        }

        template <class Instruction, class... Instructions>
        void add_impl(Instruction&& instruction, Instructions&&... instructions)
        {
            add(std::forward<Instruction>(instruction));
            add_impl(std::forward<Instructions>(instructions)...);
        }

    private:
        std::map<std::uint64_t, any_instruction> instruction_map_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP
