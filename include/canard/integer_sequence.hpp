#ifndef CANARD_INTEGER_SEQUENCE_HPP
#define CANARD_INTEGER_SEQUENCE_HPP

#include <cstddef>
#include <type_traits>

namespace canard {

    template <class T, T... Ints>
    struct integer_sequence
    {
        using value_type = T;

        static constexpr auto size() noexcept
            -> std::size_t
        {
            return sizeof...(Ints);
        }
    };

    template <std::size_t... Ints>
    using index_sequence = integer_sequence<std::size_t, Ints...>;

    namespace detail {

        template <
              class IntegerSequence
            , typename IntegerSequence::value_type N
            , typename IntegerSequence::value_type... Last
        >
        struct extend_integer_sequence;

        template <class T, T... Ints, T N, T... Last>
        struct extend_integer_sequence<integer_sequence<T, Ints...>, N, Last...>
        {
            using type = integer_sequence<T, Ints..., (Ints + N)..., Last...>;
        };

        template <class T, T N, class Enabled = void>
        struct make_integer_sequence_impl;

        template <class T, T N>
        struct make_integer_sequence_impl<
            T, N, typename std::enable_if<(N > 0) && N % 2 == 0>::type
        >
            : extend_integer_sequence<
                  typename make_integer_sequence_impl<T, N / 2>::type
                , N / 2
              >
        {
        };

        template <class T, T N>
        struct make_integer_sequence_impl<
            T, N, typename std::enable_if<(N > 0) && N % 2 != 0>::type
        >
            : extend_integer_sequence<
                  typename make_integer_sequence_impl<T, N / 2>::type
                , N / 2
                , N - 1
              >
        {
        };

        template <class T, T N>
        struct make_integer_sequence_impl<
            T, N, typename std::enable_if<N == 0>::type
        >
        {
            using type = integer_sequence<T>;
        };

    } // namespace detail

    template <class T, T N>
    using make_integer_sequence
        = typename detail::make_integer_sequence_impl<T, N>::type;

    template <std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;

    template <class... T>
    using make_index_sequence_for = make_index_sequence<sizeof...(T)>;

} // namespace canard

#endif // CANARD_INTEGER_SEQUENCE_HPP
