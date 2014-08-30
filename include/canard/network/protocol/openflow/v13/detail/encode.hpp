#ifndef CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP

#include <type_traits>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class T>
        auto encode_impl(T const& value, std::true_type)
            -> T
        {
            return detail::hton(value);
        }

        template <class T>
        auto encode_impl(T&& value, std::false_type)
            -> T&&
        {
            return value;
        }

        template <class T, class Container, class IsNWOrder = std::true_type>
        auto encode(Container& container, T const& value, IsNWOrder = IsNWOrder{})
            -> Container&
        {
            return boost::push_back(container
                    , canard::as_byte_range(encode_impl(value, IsNWOrder{})));
        }

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP
