#ifndef CANARD_NETWORK_OPENFLOW_ENCODE_HPP
#define CANARD_NETWORK_OPENFLOW_ENCODE_HPP

#include <type_traits>
#include <boost/endian/conversion.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class T, class Buffers, class IsBigEndian = std::true_type>
        auto encode(Buffers& buffers, T value)
            -> Buffers&
        {
            boost::endian::native_to_big_inplace(value);
            return buffers.push_back(value);
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ENCODE_HPP
