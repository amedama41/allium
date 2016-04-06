#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_NEXT_TABLES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_NEXT_TABLES_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace table_feature_properties {

    template <class T>
    class basic_prop_next_tables
        : private boost::equality_comparable<basic_prop_next_tables<T>>
    {
        using raw_ofp_type = v13_detail::ofp_table_feature_prop_next_tables;

    public:
        static constexpr protocol::ofp_table_feature_prop_type prop_type
            = T::prop_type;

        using next_table_id_container = std::vector<std::uint8_t>;
        using iterator = next_table_id_container::iterator;
        using const_iterator = next_table_id_container::const_iterator;

        explicit basic_prop_next_tables(next_table_id_container table_ids)
            : table_feature_prop_next_tables_{
                  prop_type
                , std::uint16_t(calc_length(table_ids))
              }
            , next_table_ids_(std::move(table_ids))
        {
        }

        explicit basic_prop_next_tables(
                std::initializer_list<std::uint8_t> table_ids)
            : basic_prop_next_tables{next_table_id_container(table_ids)}
        {
        }

        basic_prop_next_tables(basic_prop_next_tables const&) = default;

        basic_prop_next_tables(basic_prop_next_tables&& other)
            : table_feature_prop_next_tables_(
                    other.table_feature_prop_next_tables_)
            , next_table_ids_(std::move(other).next_table_ids_)
        {
            other.table_feature_prop_next_tables_.length = sizeof(raw_ofp_type);
        }

        auto operator=(basic_prop_next_tables const&)
            -> basic_prop_next_tables& = default;

        auto operator=(basic_prop_next_tables&& other)
            -> basic_prop_next_tables&
        {
            auto tmp = std::move(other);
            std::swap(table_feature_prop_next_tables_
                    , tmp.table_feature_prop_next_tables_);
            next_table_ids_.swap(tmp.next_table_ids_);
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
            return table_feature_prop_next_tables_.length;
        }

        auto next_table_ids() const noexcept
            -> next_table_id_container const&
        {
            return next_table_ids_;
        }

        auto extract_next_table_ids()
            -> next_table_id_container
        {
            table_feature_prop_next_tables_.length = sizeof(raw_ofp_type);
            return std::move(next_table_ids_);
        }

        auto begin() noexcept
            -> iterator
        {
            return next_table_ids_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return next_table_ids_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return next_table_ids_.end();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return next_table_ids_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, table_feature_prop_next_tables_);
            detail::encode_byte_array(
                    container, next_table_ids_.data(), next_table_ids_.size());
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> basic_prop_next_tables
        {
            auto const property = detail::decode<raw_ofp_type>(first, last);

            auto const id_last
                = std::next(first, property.length - sizeof(raw_ofp_type));
            auto next_table_ids = next_table_id_container(first, id_last);
            first = id_last;

            std::advance(first, v13_detail::padding_length(property.length));

            return basic_prop_next_tables{property, std::move(next_table_ids)};
        }

        friend auto operator==(
                  basic_prop_next_tables const& lhs
                , basic_prop_next_tables const& rhs)
            -> bool
        {
            return lhs.next_table_ids_ == rhs.next_table_ids_;
        }

    private:
        basic_prop_next_tables(
                  raw_ofp_type const& table_feature_prop_next_tables
                , next_table_id_container&& next_table_ids)
            : table_feature_prop_next_tables_(table_feature_prop_next_tables)
            , next_table_ids_(std::move(next_table_ids))
        {
        }

        static auto calc_length(next_table_id_container const& table_ids)
            -> std::size_t
        {
            return sizeof(raw_ofp_type)
                + table_ids.size() * sizeof(next_table_id_container::value_type);
        }

    private:
        raw_ofp_type table_feature_prop_next_tables_;
        next_table_id_container next_table_ids_;
    };

    namespace next_tables_detail {

        struct next_tables
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_NEXT_TABLES;
        };

        struct next_tables_miss
        {
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_NEXT_TABLES_MISS;
        };

    } // namespace next_tables_detail

    using next_tables = basic_prop_next_tables<next_tables_detail::next_tables>;
    using next_tables_miss
        = basic_prop_next_tables<next_tables_detail::next_tables_miss>;

} // namespace table_feature_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_NEXT_TABLES_HPP
