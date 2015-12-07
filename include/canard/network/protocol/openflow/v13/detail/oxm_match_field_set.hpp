#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_FIELD_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_FIELD_SET_HPP

#include <cstdint>
#include <map>
#include <type_traits>
#include <utility>
#include <boost/optional/optional.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/detail/add_helper.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/decode_oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        class oxm_match_field_set
        {
        public:
            template <class... OXMMatchFields, typename std::enable_if<!is_related<oxm_match_field_set, OXMMatchFields...>::value>::type* = nullptr>
            oxm_match_field_set(OXMMatchFields&&... match_fields)
                : oxm_match_field_map_{}
            {
                add_impl(std::forward<OXMMatchFields>(match_fields)...);
            }

            auto length() const
                -> std::uint16_t
            {
                using boost::adaptors::map_values;
                using boost::adaptors::transformed;
                return boost::accumulate(
                          oxm_match_field_map_ | map_values | transformed([](any_oxm_match_field const& field) { return field.length(); })
                        , std::uint16_t{0});
            }

            template <class OXMMatchField>
            void add(OXMMatchField&& field)
            {
                auto const oxm_type = field.oxm_type();
                auto const it = oxm_match_field_map_.lower_bound(oxm_type);
                if (it != oxm_match_field_map_.end() && !oxm_match_field_map_.key_comp()(oxm_type, it->first)) {
                    it->second = std::forward<OXMMatchField>(field);
                }
                else {
                    oxm_match_field_map_.emplace_hint(it, oxm_type, std::forward<OXMMatchField>(field));
                }
            }

            template <class OXMMatchField>
            auto get() const
                -> boost::optional<OXMMatchField const&>
            {
                auto const it = oxm_match_field_map_.find(OXMMatchField::oxm_type());
                if (it == oxm_match_field_map_.end()) {
                    return boost::none;
                }
                return any_cast<OXMMatchField>(it->second);
            }

            auto operator[](std::uint32_t const oxm_type) const
                -> boost::optional<any_oxm_match_field const&>
            {
                auto const it = oxm_match_field_map_.find(oxm_type);
                if (it == oxm_match_field_map_.end()) {
                    return boost::none;
                }
                return it->second;
            }

            auto begin() const
                -> boost::range_iterator<
                        boost::select_second_const_range<std::map<std::uint32_t, any_oxm_match_field>> const
                   >::type
                //-> decltype(begin(std::map<std::uint32_t, any_oxm_match_field>{} | boost::adaptors::map_values))
            {
                return boost::begin(oxm_match_field_map_ | boost::adaptors::map_values);
            }

            auto end() const
                // -> decltype(end(std::map<std::uint32_t, any_oxm_match_field>{} | boost::adaptors::map_values))
                -> boost::range_iterator<
                        boost::select_second_const_range<std::map<std::uint32_t, any_oxm_match_field>> const
                   >::type
            {
                return boost::end(oxm_match_field_map_ | boost::adaptors::map_values);
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                using boost::adaptors::map_values;
                boost::for_each(oxm_match_field_map_ | map_values, [&](any_oxm_match_field const& field) {
                    field.encode(container);
                });
                return container;
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> oxm_match_field_set
            {
                auto match_field_set = oxm_match_field_set{};
                while (first != last) {
                    decode_oxm_match_field<void>(first, last, detail::add_helper<oxm_match_field_set>{match_field_set});
                }
                return match_field_set;
            }

        private:
            void add_impl()
            {
            }

            template <class OXMMatchField, class... OXMMatchFields>
            void add_impl(OXMMatchField&& match_field, OXMMatchFields&&... match_fields)
            {
                add(std::forward<OXMMatchField>(match_field));
                add_impl(std::forward<OXMMatchFields>(match_fields)...);
            }

        private:
            std::map<std::uint32_t, any_oxm_match_field> oxm_match_field_map_;
        };

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_FIELD_SET_HPP
