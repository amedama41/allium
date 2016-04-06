#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_INSTRUCTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_INSTRUCTIONS_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/operators.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction_id.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace table_feature_properties {

    template <class T>
    class basic_prop_instructions
        : private boost::equality_comparable<basic_prop_instructions<T>>
    {
        using raw_ofp_type = v13_detail::ofp_table_feature_prop_instructions;

    public:
        using instruction_id_container = std::vector<any_instruction_id>;
        using iterator = instruction_id_container::iterator;
        using const_iterator = instruction_id_container::const_iterator;

        static constexpr protocol::ofp_table_feature_prop_type prop_type
            = T::prop_type;

        explicit basic_prop_instructions(
                instruction_id_container&& instruction_ids)
            : table_feature_prop_instructions_{
                  prop_type
                , std::uint16_t(calc_length(instruction_ids))
              }
            , instruction_ids_(std::move(instruction_ids))
        {
        }

        template <
              class... InstructionIDs
            , class = typename std::enable_if<
                !canard::is_related<
                    InstructionIDs..., basic_prop_instructions>::value
              >::type
        >
        explicit basic_prop_instructions(InstructionIDs&&... instruction_ids)
            : basic_prop_instructions{
                instruction_id_container{
                    std::forward<InstructionIDs>(instruction_ids)...
                }
              }
        {
        }

        basic_prop_instructions(basic_prop_instructions const&) = default;

        basic_prop_instructions(basic_prop_instructions&& other)
            : table_feature_prop_instructions_(
                    other.table_feature_prop_instructions_)
            , instruction_ids_(std::move(other).instruction_ids_)
        {
            other.table_feature_prop_instructions_.length
                = sizeof(raw_ofp_type);
        }

        auto operator=(basic_prop_instructions const&)
            -> basic_prop_instructions& = default;

        auto operator=(basic_prop_instructions&& other)
            -> basic_prop_instructions&
        {
            auto tmp = std::move(other);
            std::swap(table_feature_prop_instructions_
                    , tmp.table_feature_prop_instructions_);
            instruction_ids_.swap(tmp.instruction_ids_);
            return *this;
        }

        static constexpr auto type() noexcept
            -> protocol::ofp_table_feature_prop_type
        {
            return prop_type;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return table_feature_prop_instructions_.length;
        }

        auto instruction_ids() const noexcept
            -> instruction_id_container const&
        {
            return instruction_ids_;
        }

        auto extract_instruction_ids()
            -> instruction_id_container
        {
            table_feature_prop_instructions_.length = sizeof(raw_ofp_type);
            return std::move(instruction_ids_);
        }

        auto begin() noexcept
            -> iterator
        {
            return instruction_ids_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return instruction_ids_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return instruction_ids_.end();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return instruction_ids_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, table_feature_prop_instructions_);
            boost::for_each(
                    instruction_ids_, [&](any_instruction_id const& id) {
                id.encode(container);
            });
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> basic_prop_instructions
        {
            auto const property = detail::decode<raw_ofp_type>(first, last);

            auto const ids_last
                = std::next(first, property.length - sizeof(raw_ofp_type));
            constexpr auto base_id_size = sizeof(v13_detail::ofp_instruction);
            auto ids = instruction_id_container{};
            ids.reserve(std::distance(first, ids_last) / base_id_size);
            while (std::distance(first, ids_last) >= base_id_size) {
                ids.push_back(any_instruction_id::decode(first, ids_last));
            }
            if (first != ids_last) {
                throw std::runtime_error{
                    "invalid table_feature_prop_instructions length"
                };
            }

            std::advance(first, v13_detail::padding_length(property.length));

            return basic_prop_instructions{property, std::move(ids)};
        }

        friend auto operator==(
                  basic_prop_instructions const& lhs
                , basic_prop_instructions const& rhs)
            -> bool
        {
            return lhs.instruction_ids_ == rhs.instruction_ids_;
        }

    private:
        basic_prop_instructions(
                  raw_ofp_type const& table_feature_prop_instructions
                , instruction_id_container&& instruction_ids)
            : table_feature_prop_instructions_(table_feature_prop_instructions)
            , instruction_ids_(std::move(instruction_ids))
        {
        }

        static auto calc_length(
                instruction_id_container const& instruction_ids) noexcept
            -> std::size_t
        {
            return boost::accumulate(
                      instruction_ids, sizeof(raw_ofp_type)
                    , [](std::size_t const length, any_instruction_id const& id)
                      { return length + id.length(); });
        }

    private:
        raw_ofp_type table_feature_prop_instructions_;
        instruction_id_container instruction_ids_;
    };

    namespace instructions_detail {

        struct instructions
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_INSTRUCTIONS;
        };

        struct instructions_miss
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_INSTRUCTIONS_MISS;
        };

    } // namespace instructions_detail

    using instructions
        = basic_prop_instructions<instructions_detail::instructions>;
    using instructions_miss
        = basic_prop_instructions<instructions_detail::instructions_miss>;

} // namespace table_feature_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_INSTRUCTIONS_HPP

