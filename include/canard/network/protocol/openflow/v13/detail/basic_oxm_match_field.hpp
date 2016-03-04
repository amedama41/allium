#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_OXM_MATCH_FIELD_HPP

#include <cstddef>
#include <cstdint>
#include <array>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/operators.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

    namespace basic_oxm_match_field_detail {

        template <class Mask, class T>
        auto all_of(Mask const& mask, T t) noexcept
            -> bool
        {
            return boost::algorithm::all_of(
                      canard::as_byte_range(mask)
                    , [=](std::uint8_t const e) { return e == t; });
        }

        template <std::size_t N, class T>
        auto all_of(std::array<std::uint8_t, N> const& mask, T t) noexcept
            -> bool
        {
            return boost::algorithm::all_of(
                    mask, [=](std::uint8_t const e) { return e == t; });
        }

    } // namespace basic_oxm_match_field_detail

    template <class T>
    class basic_oxm_match_field
        : private boost::equality_comparable<T>
    {
    public:
        static constexpr auto oxm_type() noexcept
            -> std::uint32_t
        {
            return std::uint32_t{T::oxm_class()} << 7 | T::oxm_field();
        }

        auto oxm_header() const noexcept
            -> std::uint32_t
        {
            return (oxm_type() << 9)
                | (std::uint32_t{oxm_has_mask()} << 8)
                | oxm_length();
        }

        auto oxm_has_mask() const noexcept
            -> bool
        {
            return static_cast<bool>(derived().oxm_mask());
        }

        auto oxm_length() const noexcept
            -> std::uint8_t
        {
            using oxm_value_type = typename T::oxm_value_type;
            return oxm_has_mask()
                ? sizeof(oxm_value_type) * 2 : sizeof(oxm_value_type);
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return sizeof(oxm_header()) + oxm_length();
        }

        auto is_wildcard() const noexcept
            -> bool
        {
            return oxm_has_mask()
                ? basic_oxm_match_field_detail::all_of(
                        derived().raw_mask(), 0)
                : false;
        }

        auto is_exact() const noexcept
            -> bool
        {
            return oxm_has_mask()
                ? basic_oxm_match_field_detail::all_of(
                        derived().raw_mask(), 0xff)
                : true;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            using oxm_value_type = typename T::oxm_value_type;
            detail::encode(container, oxm_header());
            detail::encode(
                      container
                    , derived().raw_value(), sizeof(oxm_value_type)
                    , typename T::needs_byte_order_conversion{});
            if (oxm_has_mask()) {
                detail::encode(
                          container
                        , derived().raw_mask(), sizeof(oxm_value_type)
                        , typename T::needs_byte_order_conversion{});
            }
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            using oxm_value_type = typename T::oxm_value_type;
            auto const oxm_header = detail::decode<std::uint32_t>(first, last);
            auto const value = detail::decode<oxm_value_type>(
                      first, last, sizeof(oxm_value_type)
                    , typename T::needs_byte_order_conversion{});
            if (oxm_header & 0x00000100) {
                auto const mask = detail::decode<oxm_value_type>(
                          first, last, sizeof(oxm_value_type)
                        , typename T::needs_byte_order_conversion{});
                return T{value, mask};
            }
            else {
                return T{value};
            }
        }

        static void validate_oxm_header(std::uint32_t const oxm_header)
        {
            if ((oxm_header >> 16) != T::oxm_class()) {
                throw std::runtime_error{"invalid oxm class"};
            }
            if (((oxm_header >> 9) & 0x7f) == T::oxm_field()) {
                throw std::runtime_error{"invalid oxm field"};
            }
            using oxm_value_type = typename T::oxm_value_type;
            auto const expected_length = (oxm_header & 0x00000100)
                ? sizeof(oxm_value_type) * 2 : sizeof(oxm_value_type);
            if ((oxm_header & 0xff) != expected_length) {
                throw std::runtime_error{"invalid oxm length"};
            }
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> T
        {
            return T::validate(T(std::forward<Args>(args)...));
        }

        friend auto operator==(T const& lhs, T const& rhs) noexcept
            -> bool
        {
            if (lhs.is_wildcard()) {
                return rhs.is_wildcard();
            }
            if (lhs.is_exact()) {
                return rhs.is_exact() && lhs.oxm_value() == rhs.oxm_value();
            }
            return lhs.oxm_mask() == rhs.oxm_mask()
                && lhs.oxm_value() == rhs.oxm_value();
        }

    private:
        auto derived() const noexcept
            -> T const&
        {
            return static_cast<T const&>(*this);
        }
    };

} // namespace detail
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_OXM_MATCH_FIELD_HPP
