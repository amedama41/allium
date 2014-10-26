#ifndef CANARD_MPL_ADAPTED_STD_TUPLE_HPP
#define CANARD_MPL_ADAPTED_STD_TUPLE_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/mpl/clear_fwd.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/push_back_fwd.hpp>
#include <boost/mpl/push_front_fwd.hpp>

namespace boost {
namespace fusion {
namespace result_of {

    template <typename Tuple, int Index>
    struct value_of<boost::fusion::std_tuple_iterator<Tuple, Index>>
        : std::conditional<
              std::tuple_size<Tuple>::value == Index
            , boost::mpl::identity<void>
            , typename boost::fusion::extension::value_of_impl<
                typename boost::fusion::detail::tag_of<
                    boost::fusion::std_tuple_iterator<Tuple, Index>
                >::type
              >::template apply<boost::fusion::std_tuple_iterator<Tuple, Index>>
          >::type
    {};

} // namespace result_of
} // namespace fusion
} // namespace boost

namespace boost {
namespace mpl {

    template <class... T>
    struct clear<std::tuple<T...>>
    {
        using type = std::tuple<>;
    };

    template <class... SequenceElems, class T>
    struct push_back<std::tuple<SequenceElems...>, T>
    {
        using type = std::tuple<SequenceElems..., T>;
    };

    template <class... SequenceElems, class T>
    struct push_front<std::tuple<SequenceElems...>, T>
    {
        using type = std::tuple<T, SequenceElems...>;
    };

    template <class T, class... SequenceElems>
    struct pop_front<std::tuple<T, SequenceElems...>>
    {
        using type = std::tuple<SequenceElems...>;
    };

} // namespace mpl
} // namespace boost

#endif // CANARD_MPL_ADAPTED_STD_TUPLE_HPP
