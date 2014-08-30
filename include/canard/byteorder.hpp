#ifndef CANARD_BYTEORDER_HPP
#define CANARD_BYTEORDER_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <arpa/inet.h>

namespace canard {

    namespace detail {
        template <std::uint16_t N1, std::uint16_t N2>
        struct is_little_endian_impl : std::true_type {};

        template <std::uint16_t N>
        struct is_little_endian_impl<N, N> : std::false_type {};

        struct is_little_endian
            : public is_little_endian_impl<1, 2> {}; // TODO

        template <bool is_littel_endian = true>
        inline constexpr auto hton64(std::uint64_t value) noexcept
            -> std::uint64_t
        {
            return std::uint64_t{htonl(std::uint32_t(value & 0xFFFFFFFF))} << 32 | htonl(std::uint32_t(value >> 32));
        }

        template <>
        inline constexpr auto hton64<false>(std::uint64_t value) noexcept
            -> std::uint64_t
        {
            return value;
        }

        template <bool is_littel_endian = true>
        inline constexpr auto ntoh64(std::uint64_t value) noexcept
            -> std::uint64_t
        {
            return std::uint64_t{ntohl(std::uint32_t(value & 0xFFFFFFFF))} << 32 | ntohl(std::uint32_t(value >> 32));
        }

        template <>
        inline constexpr auto ntoh64<false>(std::uint64_t value) noexcept
            -> std::uint64_t
        {
            return value;
        }
    } // namespace detail

    template <class T>
    T&& hton(T&& value) noexcept = delete;

    inline constexpr auto hton(std::uint8_t value) noexcept
        -> std::uint8_t
    {
        return value;
    }

    inline constexpr auto hton(std::uint16_t value) noexcept
        -> std::uint16_t
    {
        return htons(value);
    }

    inline constexpr auto hton(std::uint32_t value) noexcept
        -> std::uint32_t
    {
        return htonl(value);
    }

    inline constexpr auto hton(std::uint64_t value) noexcept
        -> std::uint64_t
    {
        return detail::hton64<detail::is_little_endian::value>(value);
    }

    template <class T>
    T&& ntoh(T&& value) noexcept = delete;

    inline constexpr auto ntoh(std::uint8_t value) noexcept
        -> std::uint8_t
    {
        return value;
    }

    inline constexpr auto ntoh(std::uint16_t value) noexcept
        -> std::uint16_t
    {
        return ntohs(value);
    }

    inline constexpr auto ntoh(std::uint32_t value) noexcept
        -> std::uint32_t
    {
        return ntohl(value);
    }

    inline constexpr auto ntoh(std::uint64_t value) noexcept
        -> std::uint64_t
    {
        return detail::ntoh64<detail::is_little_endian::value>(value);
    }

} // namespace canard

#endif // CANARD_BYTEORDER_HPP
