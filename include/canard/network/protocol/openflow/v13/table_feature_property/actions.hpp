#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_ACTIONS_HPP

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
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/any_action_id.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace table_feature_properties {

    template <class T>
    class basic_prop_actions
        : private boost::equality_comparable<basic_prop_actions<T>>
    {
        using raw_ofp_type = v13_detail::ofp_table_feature_prop_actions;

    public:
        static constexpr protocol::ofp_table_feature_prop_type prop_type
            = T::prop_type;

        using action_id_container = std::vector<any_action_id>;
        using iterator = action_id_container::iterator;
        using const_iterator = action_id_container::const_iterator;

        explicit basic_prop_actions(action_id_container action_ids)
            : table_feature_prop_actions_{
                  prop_type
                , std::uint16_t(calc_length(action_ids))
              }
            , action_ids_(std::move(action_ids))
        {
        }

        template <class... ActionIDs
            , class = typename std::enable_if<
                !canard::is_related<ActionIDs..., basic_prop_actions>::value
              >::type
        >
        explicit basic_prop_actions(ActionIDs&&... action_ids)
            : basic_prop_actions{
                action_id_container{std::forward<ActionIDs>(action_ids)...}
              }
        {
        }

        basic_prop_actions(basic_prop_actions const&) = default;

        basic_prop_actions(basic_prop_actions&& other)
            : table_feature_prop_actions_(other.table_feature_prop_actions_)
            , action_ids_(std::move(other).action_ids_)
        {
            other.table_feature_prop_actions_.length = sizeof(raw_ofp_type);
        }

        auto operator=(basic_prop_actions const&)
            -> basic_prop_actions& = default;

        auto operator=(basic_prop_actions&& other)
            -> basic_prop_actions&
        {
            auto tmp = std::move(other);
            std::swap(table_feature_prop_actions_
                    , tmp.table_feature_prop_actions_);
            action_ids_.swap(tmp.action_ids_);
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
            return table_feature_prop_actions_.length;
        }

        auto action_ids() const noexcept
            -> action_id_container const&
        {
            return action_ids_;
        }

        auto extract_action_ids()
            -> action_id_container
        {
            table_feature_prop_actions_.length = sizeof(raw_ofp_type);
            return std::move(action_ids_);
        }

        auto begin() noexcept
            -> iterator
        {
            return action_ids_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return action_ids_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return action_ids_.end();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return action_ids_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, table_feature_prop_actions_);
            boost::for_each(action_ids_, [&](any_action_id const& id) {
                id.encode(container);
            });
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> basic_prop_actions
        {
            auto const property = detail::decode<raw_ofp_type>(first, last);

            auto const id_last
                = std::next(first, property.length - sizeof(raw_ofp_type));
            constexpr auto base_id_size
                = offsetof(v13_detail::ofp_action_header, pad);
            auto action_ids = action_id_container{};
            action_ids.reserve(std::distance(first, id_last) / base_id_size);
            while (std::distance(first, id_last) >= base_id_size) {
                action_ids.push_back(any_action_id::decode(first, id_last));
            }
            if (first != id_last) {
                throw std::runtime_error{
                    "invalid table_feature_prop_actions length"
                };
            }

            std::advance(first, v13_detail::padding_length(property.length));
            return basic_prop_actions{property, std::move(action_ids)};
        }

        friend auto operator==(
                basic_prop_actions const& lhs, basic_prop_actions const& rhs)
            -> bool
        {
            return lhs.action_ids_ == rhs.action_ids_;
        }

    private:
        basic_prop_actions(
                  raw_ofp_type const& table_feature_prop_actions
                , action_id_container&& action_ids)
            : table_feature_prop_actions_(table_feature_prop_actions)
            , action_ids_(std::move(action_ids))
        {
        }

        static auto calc_length(action_id_container const& action_ids)
            -> std::size_t
        {
            return boost::accumulate(
                      action_ids, sizeof(raw_ofp_type)
                    , [](std::size_t const length, any_action_id const& id)
                      { return length + id.length(); });
        }

    private:
        raw_ofp_type table_feature_prop_actions_;
        action_id_container action_ids_;
    };

    namespace actions_detail {

        struct write_actions
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_ACTIONS;
        };

        struct write_actions_miss
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_ACTIONS_MISS;
        };

        struct apply_actions
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_ACTIONS;
        };

        struct apply_actions_miss
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_ACTIONS_MISS;
        };

    } // namespace actions_detail

    using write_actions = basic_prop_actions<actions_detail::write_actions>;
    using write_actions_miss
        = basic_prop_actions<actions_detail::write_actions_miss>;
    using apply_actions = basic_prop_actions<actions_detail::apply_actions>;
    using apply_actions_miss
        = basic_prop_actions<actions_detail::apply_actions_miss>;

} // namespace table_feature_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_ACTIONS_HPP
