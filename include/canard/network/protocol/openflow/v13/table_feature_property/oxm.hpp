#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_OXM_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_OXM_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/operators.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_id.hpp>
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
    class basic_prop_oxm
        : private boost::equality_comparable<basic_prop_oxm<T>>
    {
        using raw_ofp_type = v13_detail::ofp_table_feature_prop_oxm;

    public:
        static constexpr protocol::ofp_table_feature_prop_type prop_type
            = T::prop_type;

        using oxm_id_container = std::vector<any_oxm_id>;
        using iterator = oxm_id_container::iterator;
        using const_iterator = oxm_id_container::const_iterator;

        explicit basic_prop_oxm(oxm_id_container oxm_ids)
            : table_feature_prop_oxm_{
                  prop_type
                , std::uint16_t(calc_length(oxm_ids))
              }
            , oxm_ids_(std::move(oxm_ids))
        {
        }

        template <
              class... OXMIDs
            , class = typename std::enable_if<
                !canard::is_related<OXMIDs..., basic_prop_oxm>::value
              >::type
        >
        explicit basic_prop_oxm(OXMIDs&&... oxm_ids)
            : basic_prop_oxm{oxm_id_container{std::forward<OXMIDs>(oxm_ids)...}}
        {
        }

        basic_prop_oxm(basic_prop_oxm const&) = default;

        basic_prop_oxm(basic_prop_oxm&& other)
            : table_feature_prop_oxm_(other.table_feature_prop_oxm_)
            , oxm_ids_(std::move(other).oxm_ids_)
        {
            other.table_feature_prop_oxm_.length = sizeof(raw_ofp_type);
        }

        auto operator=(basic_prop_oxm const&)
            -> basic_prop_oxm& = default;

        auto operator=(basic_prop_oxm&& other)
            -> basic_prop_oxm&
        {
            auto tmp = std::move(other);
            std::swap(table_feature_prop_oxm_, tmp.table_feature_prop_oxm_);
            oxm_ids_.swap(tmp.oxm_ids_);
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
            return table_feature_prop_oxm_.length;
        }

        auto oxm_ids() const noexcept
            -> oxm_id_container const&
        {
            return oxm_ids_;
        }

        auto extract_oxm_ids()
            -> oxm_id_container
        {
            table_feature_prop_oxm_.length = sizeof(raw_ofp_type);
            return std::move(oxm_ids_);
        }

        auto begin() noexcept
            -> iterator
        {
            return oxm_ids_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return oxm_ids_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return oxm_ids_.end();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return oxm_ids_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, table_feature_prop_oxm_);
            boost::for_each(oxm_ids_, [&](any_oxm_id const& id) {
                id.encode(container);
            });
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> basic_prop_oxm
        {
            auto const property = detail::decode<raw_ofp_type>(first, last);

            auto const id_last
                = std::next(first, property.length - sizeof(raw_ofp_type));
            constexpr auto base_id_size = sizeof(std::uint32_t);
            auto oxm_ids = oxm_id_container{};
            oxm_ids.reserve(std::distance(first, id_last) / base_id_size);
            while (std::distance(first, id_last) >= base_id_size) {
                oxm_ids.push_back(any_oxm_id::decode(first, id_last));
            }
            if (first != id_last) {
                throw std::runtime_error{
                    "invalid table_feature_prop_oxm length"
                };
            }

            std::advance(first, v13_detail::padding_length(property.length));

            return basic_prop_oxm{property, std::move(oxm_ids)};
        }

        friend auto operator==(
                basic_prop_oxm const& lhs, basic_prop_oxm const& rhs)
            -> bool
        {
            return lhs.oxm_ids_ == rhs.oxm_ids_;
        }

    private:
        basic_prop_oxm(
                  raw_ofp_type const& table_feature_prop_oxm
                , oxm_id_container&& oxm_ids)
            : table_feature_prop_oxm_(table_feature_prop_oxm)
            , oxm_ids_(std::move(oxm_ids))
        {
        }

        static auto calc_length(oxm_id_container const& oxm_ids) noexcept
            -> std::size_t
        {
            return boost::accumulate(
                      oxm_ids, sizeof(raw_ofp_type)
                    , [](std::size_t const length, any_oxm_id const& id)
                      { return length + id.length(); });
        }

    private:
        raw_ofp_type table_feature_prop_oxm_;
        oxm_id_container oxm_ids_;
    };

    namespace oxm_detail {

        struct match
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_MATCH;
        };

        struct wildcards
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_WILDCARDS;
        };

        struct write_setfield
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_WRITE_SETFIELD;
        };

        struct write_setfield_miss
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_WRITE_SETFIELD_MISS;
        };


        struct apply_setfield
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_APPLY_SETFIELD;
        };


        struct apply_setfield_miss
        {
            static constexpr protocol::ofp_table_feature_prop_type prop_type
                = protocol::OFPTFPT_APPLY_SETFIELD_MISS;
        };

    } // namespace oxm_detail

    using match = basic_prop_oxm<oxm_detail::match>;
    using wildcards = basic_prop_oxm<oxm_detail::wildcards>;
    using write_setfield = basic_prop_oxm<oxm_detail::write_setfield>;
    using write_setfield_miss = basic_prop_oxm<oxm_detail::write_setfield_miss>;
    using apply_setfield = basic_prop_oxm<oxm_detail::apply_setfield>;
    using apply_setfield_miss = basic_prop_oxm<oxm_detail::apply_setfield_miss>;

} // namespace table_feature_properties
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTIES_OXM_HPP
