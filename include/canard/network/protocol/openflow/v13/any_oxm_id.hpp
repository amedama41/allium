#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_property/id/oxm_id.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_oxm_id
        : private boost::equality_comparable<any_oxm_id>
    {
        using oxm_id_variant = boost::variant<oxm_id, oxm_experimenter_id>;

    public:
        template <
              class OXMID
            , class = typename std::enable_if<
                    !canard::is_related<any_oxm_id, OXMID>::value
              >::type
        >
        any_oxm_id(OXMID&& oxm_id)
            : variant_(std::forward<OXMID>(oxm_id))
        {
        }

        template <
              class OXMID
            , class = typename std::enable_if<
                    !canard::is_related<any_oxm_id, OXMID>::value
              >::type
        >
        auto operator=(OXMID&& oxm_id)
            -> any_oxm_id&
        {
            variant_ = std::forward<OXMID>(oxm_id);
            return *this;
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

        auto length() const noexcept
            -> std::uint16_t
        {
            auto visitor = detail::length_visitor{};
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
            -> any_oxm_id
        {
            auto copy_first = first;
            auto const oxm_header
                = detail::decode<std::uint32_t>(copy_first, last);
            switch (oxm_header >> 16) {
            case protocol::OFPXMC_EXPERIMENTER:
                if (std::distance(first, last)
                        < sizeof(v13_detail::ofp_oxm_experimenter_header)) {
                    throw std::runtime_error{"invalid oxm_ids length"};
                }
                return oxm_experimenter_id::decode(first, last);
            default:
                return oxm_id::decode(first, last);
            }
        }

        template <class T>
        friend auto any_cast(any_oxm_id const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_oxm_id const*)
            -> T const*;

        friend auto operator==(
                any_oxm_id const& lhs, any_oxm_id const& rhs) noexcept
            -> bool
        {
            return lhs.variant_ == rhs.variant_;
        }

    private:
        oxm_id_variant variant_;
    };

    template <class T>
    auto any_cast(any_oxm_id const& oxm_id)
        -> T const&
    {
        return boost::get<T>(oxm_id.variant_);
    }

    template <class T>
    auto any_cast(any_oxm_id const* oxm_id)
        -> T const*
    {
        return boost::get<T>(&oxm_id->variant_);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP
