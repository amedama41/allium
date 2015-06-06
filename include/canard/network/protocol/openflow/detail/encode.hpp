#ifndef CANARD_NETWORK_OPENFLOW_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_ENCODE_HPP

#include <type_traits>
#include <canard/byteorder.hpp>
#include <canard/network/protocol/openflow/detail/buffer_sequence_adaptor.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class T>
        auto encode_impl(T const& value, std::true_type)
            -> T
        {
            return hton(value);
        }

        template <class T>
        auto encode_impl(T&& value, std::false_type)
            -> T&&
        {
            return value;
        }

        template <class T, class Buffers, class IsBigEndian = std::true_type>
        auto encode(Buffers& buffers, T const& value, IsBigEndian = IsBigEndian{})
            -> Buffers&
        {
            return buffers.push_back(encode_impl(value, IsBigEndian{}));
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ENCODE_HPP
