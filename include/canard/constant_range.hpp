#ifndef CANARD_CONSTANT_RANGE_HPP
#define CANARD_CONSTANT_RANGE_HPP

#include <type_traits>
#include <utility>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/iterator.hpp>

namespace canard {

    namespace range_detail {

        namespace constant_detail {

            template <class T>
            class to_constant_value
            {
            public:
                using result_type = T;

                to_constant_value(T value)
                    : value_(std::move(value))
                {
                }

                template <class U>
                auto operator()(U&&)
                    -> T&
                {
                    return value_;
                }

                template <class U>
                auto operator()(U&&) const
                    -> T const&
                {
                    return value_;
                }

            private:
                T value_;
            };

        } // namespace constant_detail

        template <class T, class Range>
        struct constant_range
            : public boost::iterator_range<
                  boost::transform_iterator<
                      constant_detail::to_constant_value<T>
                    , typename boost::range_iterator<Range>::type
                  >
              >
        {
            using iter_t = boost::transform_iterator<
                  constant_detail::to_constant_value<T>
                , typename boost::range_iterator<Range>::type
            >;

            using base = boost::iterator_range<iter_t>;

            constant_range(Range& rng, T value)
                : base{iter_t{boost::begin(rng), constant_detail::to_constant_value<T>{std::move(value)}}
                     , iter_t{boost::end(rng), constant_detail::to_constant_value<T>{std::move(value)}}
                  }
            {
            }
        };

        template <class T>
        struct constant_holder
        {
            T value;
        };

        template <class Range, class T>
        auto operator|(Range& range, constant_holder<T> const& holder)
            -> constant_range<T, Range>
        {
            return constant_range<T, Range>{range, holder.value};
        }

        template <class Range, class T>
        auto operator|(Range const& range, constant_holder<T> const& holder)
            -> constant_range<T, Range const>
        {
            return constant_range<T, Range const>{range, holder.value};
        }

    } // namespace range_detail

    using range_detail::constant_range;

    namespace adaptors {

        template <class T>
        auto to_constant(T&& value)
            -> range_detail::constant_holder<typename std::remove_cv<typename std::remove_reference<T>::type>::type>
        {
            return {std::forward<T>(value)};
        }

        template <class Range, class T>
        auto constant(Range& range, T&& value)
            -> constant_range<typename std::remove_cv<typename std::remove_reference<T>::type>::type, Range>
        {
            using range_t = constant_range<typename std::remove_cv<typename std::remove_reference<T>::type>::type, Range>;
            return range_t{range, std::forward<T>(value)};
        }

        template <class Range, class T>
        auto constant(Range const& range, T&& value)
            -> constant_range<typename std::remove_cv<typename std::remove_reference<T>::type>::type, Range const>
        {
            using range_t = constant_range<typename std::remove_cv<typename std::remove_reference<T>::type>::type, Range const>;
            return range_t{range, std::forward<T>(value)};
        }

    } // namespace adaptors

    template <class Integer, class ValueType>
    auto make_constant_range(Integer size, ValueType value)
        -> constant_range<ValueType, boost::iterator_range<boost::range_detail::integer_iterator<Integer>> const>
        // -> decltype(boost::irange<Integer>(0, 0) | boost::adaptors::transformed(detail::to_constant_value<ValueType>{}))
    {
        return constant_range<ValueType, boost::iterator_range<boost::range_detail::integer_iterator<Integer>> const>{boost::irange(Integer{0}, size), std::move(value)};
        // using boost::adaptors::transformed;
        // return boost::irange(Integer{0}, size) | transformed(detail::to_constant_value<ValueType>{std::move(value)});
    }

} // namespace canard

#endif // CANARD_CONSTANT_RANGE_HPP
