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
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {

    template <std::size_t N, class Handler, class... Decorators>
    struct forwarder
    {
        template <class Decorator, class... Args>
        void operator()(Decorator* const d, Args&&... args) const
        {
            using next_decorator = typename std::tuple_element<
                N, std::tuple<Handler, Decorators...>
            >::type;
            static_cast<next_decorator*>(static_cast<Handler*>(d))->handle(
                      forwarder<N - 1, Handler, Decorators...>{}
                    , std::forward<Args>(args)...);
        }
    };

    template <class Handler, class... Decorators>
    struct forwarder<0, Handler, Decorators...>
    {
        template <class Decorator, class... Args>
        void operator()(Decorator* const d, Args&&... args) const
        {
            static_cast<Handler*>(d)->handle(std::forward<Args>(args)...);
        }
    };

    namespace decorator_detail {

        template <class Tuple>
        using args_indices_t = canard::make_index_sequence<
            std::tuple_size<canard::remove_cvref_t<Tuple>>::value
        >;

        template <class Map, class Decorator>
        using has_key_t
            = typename boost::fusion::result_of::has_key<Map, Decorator>::type;

        template <class Map, class Decorator>
        using at_key_t
            = typename boost::fusion::result_of::at_key<Map, Decorator>::type;

        template <class Decorator>
        struct decorator_wrapper
            : Decorator
        {
            template <class Tuple>
            decorator_wrapper(Tuple&& tuple)
                : decorator_wrapper{
                    std::forward<Tuple>(tuple), args_indices_t<Tuple>{}
                  }
            {
            }

            template <class Tuple, std::size_t... Ints>
            decorator_wrapper(Tuple&& tuple, canard::index_sequence<Ints...>)
                : Decorator{std::get<Ints>(std::forward<Tuple>(tuple))...}
            {
            }
        };

        template <class Decorator, class Map>
        auto get_args(Map& map)
            -> typename std::enable_if<
                    has_key_t<Map, Decorator>::value, at_key_t<Map, Decorator>
               >::type
        {
            return boost::fusion::at_key<Decorator>(map);
        }

        template <class Decorator, class Map>
        auto get_args(Map&)
            -> typename std::enable_if<
                    !has_key_t<Map, Decorator>::value, std::tuple<>
               >::type
        {
            return std::tuple<>{};
        }

    } // namespace decorator_detail

    template <class Decorator, class... Args>
    auto make_args(Args&&... args)
        -> boost::fusion::pair<Decorator, std::tuple<Args&&...>>
    {
        return boost::fusion::make_pair<Decorator>(
                std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <class... Decorators>
    struct decorate
        : public decorator_detail::decorator_wrapper<Decorators>...
    {
        template <class... Pairs>
        explicit decorate(Pairs&&... pairs)
            : decorate{
                boost::fusion::map<canard::remove_cvref_t<Pairs>...>{
                    std::forward<Pairs>(pairs)...
                }
              }
        {
        }

    private:
        template <class... Pairs>
        explicit decorate(boost::fusion::map<Pairs...> map)
            : decorator_detail::decorator_wrapper<Decorators>{
                decorator_detail::get_args<Decorators>(map)
              }...
        {
        }
    };

    namespace detail {

        template <class... Decorators>
        auto has_decorators_impl(decorate<Decorators...>*) -> std::true_type;
        auto has_decorators_impl(...) -> std::false_type;

        template <class Handler>
        struct has_decorators
        {
            using type
                = decltype(has_decorators_impl(static_cast<Handler*>(0)));
        };

        template <class Handler>
        using has_decorators_t = typename has_decorators<Handler>::type;

        template <class... Decorators>
        auto cast_to_decorator_impl(decorate<Decorators...> const& d)
            -> decorate<Decorators...>&;

        template <class Handler>
        struct cast_to_decorator
        {
            using type = typename std::remove_reference<
                decltype(cast_to_decorator_impl(std::declval<Handler>()))
            >::type;
        };

        template <class T>
        struct identity
        {
            using type = T;
        };

        template <class Handler>
        struct decorator
            : std::conditional<
                    has_decorators_t<Handler>::value
                  , cast_to_decorator<Handler>
                  , identity<decorate<>>
              >::type
        {
        };

        template <class Handler>
        using decorator_t = typename decorator<Handler>::type;

        template <class Decorator>
        using to_pair_t
            = boost::fusion::pair<Decorator, data_per_channel_t<Decorator>>;

        struct is_not_null
        {
            template <class T>
            struct apply
                : boost::mpl::bool_<!std::is_same<detail::null_data, T>::value>
            {};
        };

        template <class Handler, class Decorator>
        struct channel_data;

        template <class Handler, class... Decorators>
        struct channel_data<Handler, decorate<Decorators...>>
        {
            using type = typename boost::fusion::result_of::as_map<
                typename boost::fusion::result_of::filter_if<
                      boost::fusion::vector<
                            to_pair_t<Handler>, to_pair_t<Decorators>...>
                    , is_not_null
                >::type
            >::type;
        };

        template <class Handler, class Decorator = decorator_t<Handler>>
        using channel_data_t = typename channel_data<Handler, Decorator>::type;


        template <class Handler, class Decorator = decorator_t<Handler>>
        struct first_forwarder;

        template <class Handler, class... Decorators>
        struct first_forwarder<Handler, decorate<Decorators...>>
        {
            using type = forwarder<
                  std::tuple_size<std::tuple<Decorators...>>::value
                , Handler, Decorators...
            >;
        };

        template <class Handler>
        using first_forwarder_t = typename first_forwarder<Handler>::type;

        template <class Handler, class... Args>
        auto handle(Handler& handler, Args&&... args)
            -> typename std::enable_if<has_decorators_t<Handler>::value>::type
        {
            first_forwarder_t<Handler>{}(
                    std::addressof(handler), std::forward<Args>(args)...);
        }

        template <class Handler, class... Args>
        auto handle(Handler& handler, Args&&... args)
            -> typename std::enable_if<!has_decorators_t<Handler>::value>::type
        {
            handler.handle(std::forward<Args>(args)...);
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DECORATOR_HPP
