#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/optional/optional.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/v13/decoder/instruction_decoder.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction.hpp>
#include <canard/network/protocol/openflow/v13/instruction_order.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class instruction_set
        : private boost::equality_comparable<instruction_set>
    {
        using container_type = std::map<std::uint64_t, any_instruction>;

    public:
        using key_type = std::uint64_t;
        using value_type = container_type::mapped_type;
        using reference = value_type const&;
        using const_reference = value_type const&;
        using iterator = boost::range_iterator<
            boost::select_second_const_range<container_type>
        >::type;
        using const_iterator = iterator;
        using difference_type = container_type::difference_type;
        using size_type = container_type::size_type;

        template <
              class... Instructions
            , class = typename std::enable_if<
                    !is_related<instruction_set, Instructions...>::value
              >::type
        >
        instruction_set(Instructions&&... instructions)
            : instructions_{to_pair(instructions)...}
        {
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return const_iterator{instructions_.begin()};
        }

        auto end() const noexcept
            -> const_iterator
        {
            return const_iterator{instructions_.end()};
        }

        auto cbegin() const noexcept
            -> const_iterator
        {
            return const_iterator{instructions_.begin()};
        }

        auto cend() const noexcept
            -> const_iterator
        {
            return const_iterator{instructions_.end()};
        }

        auto empty() const noexcept
            -> bool
        {
            return instructions_.empty();
        }

        auto size() const noexcept
            -> size_type
        {
            return instructions_.size();
        }

        auto max_size() const noexcept
            -> size_type
        {
            return instructions_.max_size();
        }

        template <class Instruction>
        auto get() const
            -> Instruction const&
        {
            constexpr auto order = instruction_order<Instruction>::value;
            auto const it = instructions_.find(order);
            return v13::any_cast<Instruction>(it->second);
        }

        auto at(key_type const instruction_order) const
            -> const_reference
        {
            return instructions_.at(instruction_order);
        }

        template <class Instruction>
        auto at() const
            -> Instruction const&
        {
            constexpr auto order = instruction_order<Instruction>::value;
            return v13::any_cast<Instruction>(at(order));
        }

        template <class Instruction>
        auto insert(Instruction&& instruction)
            -> std::pair<const_iterator, bool>
        {
            auto const order = get_order(instruction);
            auto const result = instructions_.emplace(
                    order, std::forward<Instruction>(instruction));
            return make_result(result.first, result.second);
        }

        template <class Instruction>
        auto assign(Instruction&& instruction)
            -> std::pair<const_iterator, bool>
        {
            auto const it = instructions_.find(get_order(instruction));
            if (it == instructions_.end()) {
                return make_result(instructions_.end(), false);
            }
            it->second = std::forward<Instruction>(instruction);
            return make_result(it, true);
        }

        template <class Instruction>
        auto insert_or_assign(Instruction&& instruction)
            -> std::pair<const_iterator, bool>
        {
            auto const order = get_order(instruction);
            auto const it = instructions_.lower_bound(order);
            if (it != instructions_.end()
                    && !instructions_.key_comp()(order, it->first)) {
                it->second = std::forward<Instruction>(instruction);
                return make_result(it, false);
            }
            else {
                auto const result = instructions_.emplace_hint(
                        it, order, std::forward<Instruction>(instruction));
                return make_result(result, true);
            }
        }

        template <class Instruction>
        auto erase()
            -> size_type
        {
            constexpr auto order = instruction_order<Instruction>::value;
            return erase(order);
        }

        auto erase(key_type const instruction_order)
            -> size_type
        {
            return instructions_.erase(instruction_order);
        }

        auto erase(const_iterator it)
            -> const_iterator
        {
            return const_iterator{instructions_.erase(it.base())};
        }

        void swap(instruction_set& other)
        {
            instructions_.swap(other.instructions_);
        }

        void clear() noexcept
        {
            instructions_.clear();
        }

        auto find(key_type const instruction_order) const
            -> const_iterator
        {
            return const_iterator{instructions_.find(instruction_order)};
        }

        template <class Instruction>
        auto find() const
            -> boost::optional<Instruction const&>
        {
            constexpr auto order = instruction_order<Instruction>::value;
            auto const it = instructions_.find(order);
            if (it == instructions_.end()) {
                return boost::none;
            }
            return v13::any_cast<Instruction>(it->second);
        }

        auto length() const noexcept
            -> std::size_t
        {
            using boost::adaptors::transformed;
            return boost::accumulate(
                      *this
                    | transformed([](const_reference e) { return e.length(); })
                    , std::size_t{0});
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            boost::for_each(*this, [&](const_reference instruction) {
                instruction.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_set
        {
            auto instructions = container_type{};
            while (std::distance(first, last)
                    >= sizeof(v13_detail::ofp_instruction)) {
                instruction_decoder::decode<void>(
                        first, last, instruction_inserter{instructions});
            }
            if (first != last) {
                throw std::runtime_error{"invalid instruction set length"};
            }
            return instruction_set{std::move(instructions)};
        }

        friend auto operator==(
                instruction_set const& lhs, instruction_set const& rhs)
            -> bool
        {
            return lhs.instructions_ == rhs.instructions_;
        }

    private:
        explicit instruction_set(container_type&& instructions)
            : instructions_(std::move(instructions))
        {
        }

        template <class Instruction>
        static auto to_pair(Instruction&& instruction)
            -> container_type::value_type
        {
            auto const instruction_order = get_order(instruction);
            return container_type::value_type{
                instruction_order, std::forward<Instruction>(instruction)
            };
        }

        static auto make_result(container_type::iterator it, bool const result)
            -> std::pair<const_iterator, bool>
        {
            return std::make_pair(const_iterator{it}, result);
        }

        struct instruction_inserter
        {
            template <class Instruction>
            void operator()(Instruction&& instruction) const
            {
                auto const order = get_order(instruction);
                auto const result = instructions.emplace(
                        order, std::forward<Instruction>(instruction));
                if (!result.second) {
                    throw std::runtime_error{"duplicated instruction type"};
                }
            }

            container_type& instructions;
        };

    private:
        container_type instructions_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_SET_HPP
