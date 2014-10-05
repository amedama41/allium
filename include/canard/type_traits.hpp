#ifndef CANARD_TYPE_TRAITS_HPP
#define CANARD_TYPE_TRAITS_HPP

#include <type_traits>

namespace canard {

    template <class T>
    struct remove_cv_and_reference
        : std::remove_cv<typename std::remove_reference<T>::type>
    {
    };

    template <class T>
    using remove_cv_and_reference_t = typename remove_cv_and_reference<T>::type;

    template <class T, class... U>
    struct is_related : std::false_type {};

    template <class T, class U>
    struct is_related<T, U> : std::is_same<
          typename remove_cv_and_reference<T>::type
        , typename remove_cv_and_reference<U>::type
    > {};

} // namespace canard

#endif // CANARD_TYPE_TRAITS_HPP
