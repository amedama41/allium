#ifndef CANARD_NETWORK_OPENFLOW_V13_LENGTH_UTILITY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_LENGTH_UTILITY_HPP

#include <cstdint>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

    inline auto exact_length(std::uint16_t const length)
        -> std::uint16_t
    {
        return (length + 7) / 8 * 8;
    }

    inline auto padding_length(std::uint16_t const length)
        -> std::uint16_t
    {
        return exact_length(length) - length;
    }

} // namespace v13
} // namespace detail

namespace v13 {
namespace v13_detail {

    using detail::v13::exact_length;
    using detail::v13::padding_length;

} // namespace v13_detail
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_LENGTH_UTILITY_HPP
