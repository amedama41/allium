#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_MATCH_FIELD_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field_list.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_oxm_match_field
    {
        using oxm_match_field_variant = boost::make_variant_over<
            default_oxm_match_field_list
        >::type;

    public:
        template <class OXMMatchField, typename std::enable_if<!is_related<any_oxm_match_field, OXMMatchField>::value>::type* = nullptr>
        any_oxm_match_field(OXMMatchField&& field)
            : variant_(std::forward<OXMMatchField>(field))
        {
        }

        template <class OXMMatchField, typename std::enable_if<!is_related<any_oxm_match_field, OXMMatchField>::value>::type* = nullptr>
        auto operator=(OXMMatchField&& field)
            -> any_oxm_match_field&
        {
            variant_ = std::forward<OXMMatchField>(field);
            return *this;
        }

        auto length() const
            -> std::uint16_t
        {
            auto visitor = detail::length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_type() const
            -> std::uint32_t
        {
            auto visitor = v13_detail::oxm_type_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_header() const
            -> std::uint32_t
        {
            auto visitor = v13_detail::oxm_header_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_has_mask() const
            -> bool
        {
            auto visitor = v13_detail::oxm_has_mask_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_length() const
            -> std::uint8_t
        {
            auto visitor = v13_detail::oxm_length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto wildcard() const
            -> bool
        {
            auto visitor = v13_detail::wildcard_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto visitor = detail::encoding_visitor<Container>{container};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class T>
        friend auto any_cast(any_oxm_match_field const& field)
            -> T const&;

        template <class T>
        friend auto any_cast(any_oxm_match_field const* field)
            -> T const*;

    private:
        oxm_match_field_variant variant_;
    };

    template <class T>
    auto any_cast(any_oxm_match_field const& field)
        -> T const&
    {
        return boost::get<T>(field.variant_);
    }

    template <class T>
    auto any_cast(any_oxm_match_field const* field)
        -> T const*
    {
        return boost::get<T>(&field->variant_);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_MATCH_FIELD_HPP
