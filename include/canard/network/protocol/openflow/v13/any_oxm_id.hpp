#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_id.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_oxm_id
    {
        using oxm_id_variant = boost::variant<oxm_id, oxm_experimenter_id>;

    public:
        template <class OXMID, typename std::enable_if<!is_related<any_oxm_id, OXMID>::value>::type* = nullptr>
        any_oxm_id(OXMID&& oxm_id)
            : variant_(std::forward<OXMID>(oxm_id))
        {
        }

        template <class OXMID, typename std::enable_if<!is_related<any_oxm_id, OXMID>::value>::type* = nullptr>
        auto operator=(OXMID&& oxm_id)
            -> any_oxm_id&
        {
            variant_ = std::forward<OXMID>(oxm_id);
            return *this;
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

        auto length() const
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
            auto const oxm_header = detail::decode<std::uint32_t>(copy_first, last);
            if ((oxm_header >> 16) == protocol::OFPXMC_EXPERIMENTER) {
                return oxm_experimenter_id::decode(first, last);
            }
            return oxm_id::decode(first, last);
        }

        template <class T>
        friend auto any_cast(any_oxm_id const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_oxm_id const*)
            -> T const*;

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
