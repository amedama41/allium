#ifndef CANARD_NETWORK_OPENFLOW_DECODE_HPP
#define CANARD_NETWORK_OPENFLOW_DECODE_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <boost/endian/conversion.hpp>
#include <boost/range/algorithm_ext/overwrite.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/as_byte_range.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class T>
        void decode_impl(T& value, std::true_type)
        {
            boost::endian::big_to_native_inplace(value);
        }

        template <class T>
        void decode_impl(T&, std::false_type)
        {
        }

        template <class T, class Iterator
                , class NeedsEndianConversion = std::true_type>
        auto decode(Iterator& first, Iterator last
                  , std::size_t const size = sizeof(T)
                  , NeedsEndianConversion = NeedsEndianConversion{})
            -> T
        {
            auto value = T{};
            boost::overwrite(
                      boost::make_iterator_range_n(first, size)
                    , canard::as_byte_range(value));
            std::advance(first, size);
            detail::decode_impl(value, NeedsEndianConversion{});
            return value;
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DECODE_HPP
