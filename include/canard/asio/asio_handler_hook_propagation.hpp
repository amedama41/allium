#ifndef CANARD_ASIO_ASIO_HANDLER_HOOK_PROPAGATION_HPP
#define CANARD_ASIO_ASIO_HANDLER_HOOK_PROPAGATION_HPP

#include <cstddef>
#include <memory>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/handler_continuation_hook.hpp>
#include <boost/asio/handler_invoke_hook.hpp>

namespace canard {

    namespace detail {

        template <class Derived>
        struct asio_handler_hook_propagation_base
        {
            friend auto asio_handler_allocate(
                    std::size_t const size, Derived* const h)
                -> void*
            {
                using boost::asio::asio_handler_allocate;
                return asio_handler_allocate(
                        size, std::addressof(h->handler()));
            }

            friend void asio_handler_deallocate(
                        void* const pointer, std::size_t const size
                      , Derived* const h)
            {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(
                        pointer, size, std::addressof(h->handler()));
            }

            friend auto asio_handler_is_continuation(Derived* const h)
                -> bool
            {
                using boost::asio::asio_handler_is_continuation;
                return asio_handler_is_continuation(
                        std::addressof(h->handler()));
            }
        };

    } // namespace detail

    struct hook_invoke_propagation {};
    struct no_propagation_hook_invoke {};

    template <class Derived, class InvokePolicy = hook_invoke_propagation>
    struct asio_handler_hook_propagation
        : detail::asio_handler_hook_propagation_base<Derived>
    {
        template <class Function>
        friend void asio_handler_invoke(Function&& function, Derived* const h)
        {
            using boost::asio::asio_handler_invoke;
            asio_handler_invoke(
                      std::forward<Function>(function)
                    , std::addressof(h->handler()));
        }
    };

    template <class Derived>
    struct asio_handler_hook_propagation<Derived, no_propagation_hook_invoke>
        : detail::asio_handler_hook_propagation_base<Derived>
    {
    };

} // namespace canard

#endif // CANARD_ASIO_ASIO_HANDLER_HOOK_PROPAGATION_HPP
