#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP

#include <cstdint>
#include <iterator>
#include <tuple>
#include <utility>
#include <boost/format.hpp>
#include <boost/variant/variant.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_properties.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace table_feature_properties {

        using property_list = std::tuple<
              instructions, instructions_miss
            , next_tables, next_tables_miss
            , write_actions, write_actions_miss, apply_actions, apply_actions_miss
            , match, wildcards
            , write_setfield, write_setfield_miss, apply_setfield, apply_setfield_miss
        >;
        using variant = boost::make_variant_over<property_list>::type;

        template <class ReturnType, class Iterator, class Function>
        inline auto decode(Iterator& first, Iterator last, Function&& function)
            -> ReturnType
        {
            auto copy_first = first;
            auto const header = detail::decode<v13_detail::ofp_table_feature_prop_header>(copy_first, last);
            switch (header.type) {
#           define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE(z, N, _) \
            case std::tuple_element<N, property_list>::type::prop_type: \
                return function(std::tuple_element<N, property_list>::type::decode(first, last));
            static_assert(std::tuple_size<property_list>::value == 14, "");
            BOOST_PP_REPEAT(14, CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE
            default:
                // TODO
                std::advance(first, v13_detail::exact_length(header.length));
                throw std::runtime_error{(boost::format{"%1%: prop_type(%2%) is unknwon"} % __func__ % header.type).str()};
            }
        }

        namespace table_feature_properties_detail {

            struct to_property_variant
            {
                template <class T>
                auto operator()(T&& t) const
                    -> variant
                {
                    return variant{std::forward<T>(t)};
                }
            };

        } // namespace table_feature_properties_detail

        template <class Iterator>
        inline auto decode(Iterator& first, Iterator last)
            -> variant
        {
            return decode<variant>(first, last, table_feature_properties_detail::to_property_variant{});
        }

    } // namespace table_feature_properties

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP
