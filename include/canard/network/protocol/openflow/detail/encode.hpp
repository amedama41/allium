#ifndef CANARD_NETWORK_OPENFLOW_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_ENCODE_HPP

#include <cstddef>
#include <type_traits>
#include <boost/endian/conversion.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/as_byte_range.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class T>
        void encode_impl(T& value, std::true_type)
        {
            boost::endian::native_to_big_inplace(value);
        }

        template <class T>
        void encode_impl(T&, std::false_type)
        {
        }

        template <class T, class Buffer
                , class NeedsEndianConversion = std::true_type>
        auto encode(Buffer& buffer, T value, std::size_t const size = sizeof(T)
                  , NeedsEndianConversion = NeedsEndianConversion{})
            -> Buffer&
        {
            detail::encode_impl(value, NeedsEndianConversion{});
            return boost::push_back(buffer, canard::as_byte_range(value, size));
        }

        template <class Buffer>
        auto encode_byte_array(
                  Buffer& buffer
                , unsigned char const* const first, std::size_t const size)
            -> Buffer&
        {
            return boost::push_back(
                    buffer, boost::make_iterator_range_n(first, size));
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ENCODE_HPP
