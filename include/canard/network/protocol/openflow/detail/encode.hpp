#ifndef CANARD_NETWORK_OPENFLOW_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_ENCODE_HPP

#include <type_traits>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/byteorder.hpp>

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

        template <class T, class Container, class IsBigEndian = std::true_type>
        auto encode(Container& container, T const& value, IsBigEndian = IsBigEndian{})
            -> Container&
        {
            return boost::push_back(container
                    , canard::as_byte_range(encode_impl(value, IsBigEndian{})));
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ENCODE_HPP
