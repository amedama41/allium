#ifndef CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP

#include <type_traits>
#include <boost/endian/conversion.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <class T>
        void encode_impl(T& value, std::true_type)
        {
            boost::endian::native_to_big_inplace(value);
        }

        template <class T>
        void encode_impl(T&, std::false_type)
        {
        }

        template <class T, class Container, class IsNWOrder = std::true_type>
        auto encode(Container& container, T value, IsNWOrder = IsNWOrder{})
            -> Container&
        {
            encode_impl(value, IsNWOrder{});
            return boost::push_back(
                      container
                    , canard::as_byte_range(value));
        }

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ENCODE_HPP
