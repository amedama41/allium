#ifndef CANARD_NETWORK_OPENFLOW_DECORATOR_HPP
#define CANARD_NETWORK_OPENFLOW_DECORATOR_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/fusion/algorithm/transformation/filter_if.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/map/convert.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/mpl/bool.hpp>
#include <canard/integer_sequence.hpp>
#include <canard/network/protocol/openflow/data_per_channel.hpp>

namespace canard {
namespace network {
namespace openflow {

  namespace decorator_detail {

    template <class Tuple>
    using args_indices_t = canard::make_index_sequence<
      std::tuple_size<typename std::decay<Tuple>::type>::value
    >;

    template <class Map, class Decorator>
    using has_key_t
      = typename boost::fusion::result_of::has_key<Map, Decorator>::type;

    template <class Map, class Decorator>
    using at_key_t
      = typename boost::fusion::result_of::at_key<Map, Decorator>::type;

    template <template <class> class T>
    struct quote {};

    template <class, class, template <class> class... >
    struct decorate_impl;

    template <class, std::size_t>
    struct forwarder;

    template <
        class Handler
      , class T, std::size_t... Ints, template <class> class... Decorators
    >
    auto get_all_decorators_impl(
        decorate_impl<T, canard::index_sequence<Ints...>, Decorators...> const&)
      -> std::tuple<T, Decorators<forwarder<T, Ints>>...>;
    template <class Handler>
    auto get_all_decorators_impl(...)
      -> std::tuple<Handler>;

    template <class T>
    struct get_all_decorators
    {
      using type = decltype(get_all_decorators_impl<T>(std::declval<T>()));
      static_assert(
            std::is_same<typename std::tuple_element<0, type>::type, T>::value
          , "Wrong use of decorate: "
            "first parameter of decorate must be handler type");
    };

    template <class Handler, std::size_t NextDecoratorIndex>
    struct forwarder
    {
      template <class... Args>
      void forward(Args&&... args)
      {
        using next_decorator = typename std::tuple_element<
            NextDecoratorIndex, typename get_all_decorators<Handler>::type
        >::type;
        auto const next
          = static_cast<next_decorator*>(static_cast<Handler*>(this));
        next->next_decorator::handle(std::forward<Args>(args)...);
      }
    };

    template <class Decorator>
    struct decorator_wrapper
      : Decorator
    {
      template <class Tuple>
      decorator_wrapper(Tuple&& tuple)
        : decorator_wrapper{std::forward<Tuple>(tuple), args_indices_t<Tuple>{}}
      {
      }

      template <class Tuple, std::size_t... Ints>
      decorator_wrapper(Tuple&& tuple, canard::index_sequence<Ints...>)
        : Decorator{std::get<Ints>(std::forward<Tuple>(tuple))...}
      {
      }
    };

    template <bool B, class T>
    using enable_if_t = typename std::enable_if<B, T>::type;

    template <class Decorator, class Map>
    auto get_args(Map& map)
      -> enable_if_t<has_key_t<Map, Decorator>::value, at_key_t<Map, Decorator>>
    {
      return boost::fusion::at_key<Decorator>(map);
    }

    template <class Decorator, class Map>
    auto get_args(Map&)
      -> enable_if_t<!has_key_t<Map, Decorator>::value, std::tuple<>>
    {
      return std::tuple<>{};
    }

    template <
      class Handler, std::size_t... Ints, template <class> class... Decorators
    >
    struct decorate_impl<
      Handler, canard::index_sequence<Ints...>, Decorators...
    >
      : public decorator_wrapper<Decorators<forwarder<Handler, Ints>>>...
    {
      template <class... Pairs>
      explicit decorate_impl(Pairs&&... pairs)
        : decorate_impl{
            boost::fusion::map<typename std::decay<Pairs>::type...>{
              std::forward<Pairs>(pairs)...
            }
          }
      {
      }

    private:
      template <class... Pairs>
      explicit decorate_impl(boost::fusion::map<Pairs...> map)
        : decorator_wrapper<Decorators<forwarder<Handler, Ints>>>{
            get_args<quote<Decorators>>(map)
          }...
      {
      }
    };


    template <class Decorator>
    using get_channel_data_t = typename data_per_channel<Decorator>::type;

    template <class Tuple>
    struct to_pair_sequence;

    template <class... Decorators>
    struct to_pair_sequence<std::tuple<Decorators...>>
    {
      using type = boost::fusion::vector<
        boost::fusion::pair<Decorators, get_channel_data_t<Decorators>>...
      >;
    };

    struct is_not_null
    {
      template <class T>
      struct apply
        : boost::mpl::bool_<!std::is_same<detail::null_data, T>::value>
      {};
    };

  } // namespace decorator_detail

  namespace detail {

    template <class Handler>
    struct channel_data
    {
      using type = typename boost::fusion::result_of::as_map<
        typename boost::fusion::result_of::filter_if<
            typename decorator_detail::to_pair_sequence<
              typename decorator_detail::get_all_decorators<Handler>::type
            >::type
          , decorator_detail::is_not_null
        >::type
      >::type;
    };

    template <class Handler>
    using channel_data_t = typename channel_data<Handler>::type;

    template <class Handler, class... Args>
    void handle(Handler& handler, Args&&... args)
    {
      using decorators
        = typename decorator_detail::get_all_decorators<Handler>::type;
      using lowest_decorator = typename std::tuple_element<
        std::tuple_size<decorators>::value - 1, decorators
      >::type;
      static_cast<lowest_decorator&>(handler).handle(
          std::forward<Args>(args)...);
    }

  } // namespace detail

  template <template <class> class Decorator, class... Args>
  auto make_args(Args&&... args)
    -> boost::fusion::pair<
         decorator_detail::quote<Decorator>, std::tuple<Args&&...>
       >
  {
    return boost::fusion::make_pair<decorator_detail::quote<Decorator>>(
        std::forward_as_tuple(std::forward<Args>(args)...));
  }

  template <class Handler, template <class> class... Decorators>
  class decorate : public decorator_detail::decorate_impl<
      Handler
    , canard::make_index_sequence<sizeof...(Decorators)>
    , Decorators...
  >
  {
    using decorator_detail::decorate_impl<
      Handler, canard::make_index_sequence<sizeof...(Decorators)>, Decorators...
    >::decorate_impl;
  };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DECORATOR_HPP
