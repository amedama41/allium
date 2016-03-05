#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_MATCH_FIELD_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/network/protocol/openflow/detail/construct.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/decoder/oxm_match_field_decoder.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_oxm_match_field
        : private boost::equality_comparable<any_oxm_match_field>
    {
        using oxm_match_field_variant = boost::make_variant_over<
            default_oxm_match_field_list
        >::type;

    public:
        using type_list = default_oxm_match_field_list;

        template <
              class OXMMatchField
            , typename std::enable_if<
                    !is_related<any_oxm_match_field, OXMMatchField>::value
              >::type* = nullptr
        >
        any_oxm_match_field(OXMMatchField&& field)
            : variant_(std::forward<OXMMatchField>(field))
        {
        }

        template <
              class OXMMatchField
            , typename std::enable_if<
                    !is_related<any_oxm_match_field, OXMMatchField>::value
              >::type* = nullptr
        >
        auto operator=(OXMMatchField&& field)
            -> any_oxm_match_field&
        {
            variant_ = std::forward<OXMMatchField>(field);
            return *this;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            auto visitor = detail::length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_class() const noexcept
            -> protocol::ofp_oxm_class
        {
            auto visitor = detail::v13::oxm_class_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_field() const noexcept
            -> std::uint8_t
        {
            auto visitor = detail::v13::oxm_field_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_type() const noexcept
            -> std::uint32_t
        {
            auto visitor = detail::v13::oxm_type_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_header() const noexcept
            -> std::uint32_t
        {
            auto visitor = detail::v13::oxm_header_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_has_mask() const noexcept
            -> bool
        {
            auto visitor = detail::v13::oxm_has_mask_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto oxm_length() const noexcept
            -> std::uint8_t
        {
            auto visitor = detail::v13::oxm_length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto is_wildcard() const noexcept
            -> bool
        {
            auto visitor = detail::v13::is_wildcard_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto is_exact() const noexcept
            -> bool
        {
            auto visitor = detail::v13::is_exact_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto visitor = detail::encoding_visitor<Container>{container};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> any_oxm_match_field
        {
            return oxm_match_field_decoder::decode<any_oxm_match_field>(
                    first, last, detail::construct<any_oxm_match_field>{});
        }

        friend auto operator==(
                any_oxm_match_field const& lhs, any_oxm_match_field const& rhs)
            -> bool;

        template <class T>
        friend auto any_cast(any_oxm_match_field const& field)
            -> T const&;

        template <class T>
        friend auto any_cast(any_oxm_match_field const* field)
            -> T const*;

    private:
        oxm_match_field_variant variant_;
    };

    inline auto operator==(
            any_oxm_match_field const& lhs, any_oxm_match_field const& rhs)
        -> bool
    {
        return lhs.variant_ == rhs.variant_;
    }

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
