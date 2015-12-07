#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/constant_range.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/detail/oxm_match_field_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class oxm_match
    {
    public:
        static protocol::ofp_match_type const match_type = protocol::OFPMT_OXM;
        static std::uint16_t const match_base_length = offsetof(v13_detail::ofp_match, pad);

        template <class... OXMMatchFields, typename std::enable_if<!is_related<oxm_match, OXMMatchFields...>::value>::type* = nullptr>
        oxm_match(OXMMatchFields&&... oxm_fields)
            : oxm_match_fields_{std::forward<OXMMatchFields>(oxm_fields)...}
        {
        }

        auto type() const
            -> protocol::ofp_match_type
        {
            return match_type;
        }

        auto length() const
            -> std::uint16_t
        {
            return match_base_length + oxm_match_fields_.length();
        }

        template <class OXMMatchField>
        void add(OXMMatchField&& field)
        {
            oxm_match_fields_.add(std::forward<OXMMatchField>(field));
        }

        template <class OXMMatchField>
        auto get() const
            -> boost::optional<OXMMatchField const&>
        {
            return oxm_match_fields_.get<OXMMatchField>();
        }

        auto operator[](std::uint32_t const oxm_type) const
            -> boost::optional<any_oxm_match_field const&>
        {
            return oxm_match_fields_[oxm_type];
        }

        auto begin() const
            -> decltype(v13_detail::oxm_match_field_set{}.begin())
        {
            return oxm_match_fields_.begin();
        }

        auto end() const
            -> decltype(v13_detail::oxm_match_field_set{}.end())
        {
            return oxm_match_fields_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            v13_detail::encode(container, std::uint16_t{match_type});
            v13_detail::encode(container, length());
            return boost::push_back(oxm_match_fields_.encode(container)
                    , canard::make_constant_range(v13_detail::padding_length(length()), 0));
        }

    private:
        oxm_match(v13_detail::oxm_match_field_set&& oxm_field)
            : oxm_match_fields_{std::move(oxm_field)}
        {
        }

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_match
        {
            auto const type = v13_detail::decode<std::uint16_t>(first, last);
            if (type != match_type) {
                throw 1;
            }
            auto const length = v13_detail::decode<std::uint16_t>(first, last);
            auto oxm_fields = v13_detail::oxm_match_field_set::decode(first, std::next(first, length - match_base_length));
            if (length != match_base_length + oxm_fields.length()) {
                throw 2;
            }
            std::advance(first, v13_detail::padding_length(length));
            return oxm_match{std::move(oxm_fields)};
        }

    private:
        v13_detail::oxm_match_field_set oxm_match_fields_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP
