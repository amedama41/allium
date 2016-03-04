#ifndef CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP
#define CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

auto operator"" _bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

}

#endif // CANARD_NETWORK_OPENFLOW_TEST_UTILITY_HPP
