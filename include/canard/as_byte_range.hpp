#ifndef CANARD_AS_BYTE_RANGE_HPP
#define CANARD_AS_BYTE_RANGE_HPP

#include <memory>
#include <type_traits>
#include <boost/range/iterator_range.hpp>

namespace canard {

    namespace byte_range_detail {

        template <
              class T
            , class = typename std::enable_if<
                            std::is_standard_layout<T>::value>::type
        >
        auto as_byte_range(T& t, std::size_t const size = sizeof(T))
            -> boost::iterator_range<unsigned char*>
        {
            return boost::make_iterator_range_n(
                      reinterpret_cast<unsigned char*>(std::addressof(t))
                    , size);
        }

        template <
              class T
            , class = typename std::enable_if<
                            std::is_standard_layout<T>::value>::type
        >
        auto as_byte_range(T const& t, std::size_t const size = sizeof(T))
            -> boost::iterator_range<unsigned char const*>
        {
            return boost::make_iterator_range_n(
                    reinterpret_cast<unsigned char const*>(std::addressof(t))
                  , size);
        }

    } // namespace byte_range_detail

    using byte_range_detail::as_byte_range;

} // namespace canard

#endif // CANARD_AS_BYTE_RANGE_HPP
