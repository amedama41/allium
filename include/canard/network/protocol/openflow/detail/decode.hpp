#ifndef CANARD_NETWORK_OPENFLOW_DECODE_HPP
#define CANARD_NETWORK_OPENFLOW_DECODE_HPP

#include <iterator>
#include <type_traits>
#include <boost/range/algorithm_ext/overwrite.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class T>
        auto decode_impl(T const& value, std::true_type)
            -> T
        {
            return ntoh(value);
        }

        template <class T>
        auto decode_impl(T&& value, std::false_type)
            -> T&&
        {
            return value;
        }

        template <class T, class Iterator, class IsNWOrder = std::true_type>
        auto decode(Iterator& first, Iterator last, IsNWOrder = IsNWOrder{})
            -> T
        {
            auto value = T{};
            boost::overwrite(boost::make_iterator_range(first, std::next(first, sizeof(value)))
                    , canard::as_byte_range(value));
            std::advance(first, sizeof(value));
            return decode_impl(value, IsNWOrder{});
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DECODE_HPP
