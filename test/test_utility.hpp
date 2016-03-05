#ifndef CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP
#define CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <canard/mac_address.hpp>

inline auto operator"" _bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

inline auto operator"" _mac(char const* const str, std::size_t const size)
    -> canard::mac_address
{
    if (size != 6) {
        throw std::runtime_error{"invalid size mac address"};
    }
    auto const bytes = reinterpret_cast<unsigned char const*>(str);
    return canard::mac_address{
        {{bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]}}
    };
}

inline auto operator"" _ipv4(char const* const ipv4, std::size_t)
    -> boost::asio::ip::address_v4
{
    return boost::asio::ip::address_v4::from_string(ipv4);
}

inline auto operator"" _ipv6(char const* const ipv6, std::size_t)
    -> boost::asio::ip::address_v6
{
    return boost::asio::ip::address_v6::from_string(ipv6);
}

#endif // CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP
