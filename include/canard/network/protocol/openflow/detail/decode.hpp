#ifndef CANARD_NETWORK_OPENFLOW_DECODE_HPP
#define CANARD_NETWORK_OPENFLOW_DECODE_HPP

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

        template <class T, class Iterator>
        auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto value = T{};
            boost::overwrite(
                      boost::make_iterator_range(first, std::next(first, sizeof(value)))
                    , canard::as_byte_range(value));
            std::advance(first, sizeof(value));
            boost::endian::big_to_native_inplace(value);
            return value;
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DECODE_HPP
