#ifndef CANARD_AS_BYTE_RANGE_HPP
#define CANARD_AS_BYTE_RANGE_HPP

#include <boost/range/iterator_range.hpp>

namespace canard {

    namespace byte_range_detail {

        template <class CharT>
        class byte_range
            : public boost::iterator_range<CharT*>
        {
        public:
            byte_range(CharT* first, CharT* last)
                : boost::iterator_range<CharT*>{first, last}
            {
            }
        };

        template <class T, typename std::enable_if<std::is_trivially_copyable<T>::value>::type* = nullptr>
        auto as_byte_range(T& t)
            -> byte_range<char>
        {
            return {
                  reinterpret_cast<char*>(&t)
                , reinterpret_cast<char*>(&t) + sizeof(t)
            };
        }

        template <class T, typename std::enable_if<std::is_trivially_copyable<T>::value>::type* = nullptr>
        auto as_byte_range(T const& t)
            -> byte_range<const char>
        {
            return {
                  reinterpret_cast<char const*>(&t)
                , reinterpret_cast<char const*>(&t) + sizeof(t)
            };
        }

    } // namespace byte_range_detail

    using byte_range_detail::as_byte_range;

} // namespace canard

#endif // CANARD_AS_BYTE_RANGE_HPP
