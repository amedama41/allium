#ifndef CANARD_ASIO_BIND_HANDLER_HPP
#define CANARD_ASIO_BIND_HANDLER_HPP

#include <utility>
#include <memory>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/handler_continuation_hook.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace detail {

    namespace bind_handler_detail {

        template <class Handler, class Arg1>
        class binder1
        {
        public:
            binder1(Handler handler, Arg1 arg1)
                : handler_(std::move(handler))
                , arg1_(std::move(arg1))
            {
            }

            void operator()()
            {
                handler_(static_cast<Arg1 const&>(arg1_));
            }

            auto handler()
                -> Handler&
            {
                return handler_;
            }

        private:
            Handler handler_;
            Arg1 arg1_;
        };

        template <class Handler, class Arg1>
        auto asio_handler_allocate(std::size_t const size, binder1<Handler, Arg1>* const this_handler)
            -> void*
        {
            using boost::asio::asio_handler_allocate;
            return asio_handler_allocate(size, std::addressof(this_handler->handler()));
        }

        template <class Handler, class Arg1>
        void asio_handler_deallocate(void* const pointer, std::size_t const size, binder1<Handler, Arg1>* const this_handler)
        {
            using boost::asio::asio_handler_deallocate;
            asio_handler_deallocate(pointer, size, std::addressof(this_handler->handler()));
        }

        template <class Handler, class Arg1>
        auto asio_handler_is_continuation(binder1<Handler, Arg1>* const this_handler)
            -> bool
        {
            using boost::asio::asio_handler_is_continuation;
            return asio_handler_is_continuation(std::addressof(this_handler->handler()));
        }

        template <class Function, class Handler, class Arg1>
        void asio_handler_invoke(Function&& function, binder1<Handler, Arg1>* const this_handler)
        {
            using boost::asio::asio_handler_invoke;
            asio_handler_invoke(std::forward<Function>(function), std::addressof(this_handler->handler()));
        }


        template <class Handler, class Arg1, class Arg2>
        class binder2
        {
        public:
            binder2(Handler handler, Arg1 arg1, Arg2 arg2)
                : handler_(std::move(handler))
                , arg1_(std::move(arg1))
                , arg2_(std::move(arg2))
            {
            }

            void operator()()
            {
                handler_(static_cast<Arg1 const&>(arg1_), static_cast<Arg2 const&>(arg2_));
            }

            auto handler()
                -> Handler&
            {
                return handler_;
            }

        private:
            Handler handler_;
            Arg1 arg1_;
            Arg2 arg2_;
        };

        template <class Handler, class Arg1, class Arg2>
        auto asio_handler_allocate(std::size_t const size, binder2<Handler, Arg1, Arg2>* const this_handler)
            -> void*
        {
            using boost::asio::asio_handler_allocate;
            return asio_handler_allocate(size, std::addressof(this_handler->handler()));
        }

        template <class Handler, class Arg1, class Arg2>
        void asio_handler_deallocate(void* const pointer, std::size_t const size, binder2<Handler, Arg1, Arg2>* const this_handler)
        {
            using boost::asio::asio_handler_deallocate;
            asio_handler_deallocate(pointer, size, std::addressof(this_handler->handler()));
        }

        template <class Handler, class Arg1, class Arg2>
        auto asio_handler_is_continuation(binder2<Handler, Arg1, Arg2>* const this_handler)
            -> bool
        {
            using boost::asio::asio_handler_is_continuation;
            return asio_handler_is_continuation(std::addressof(this_handler->handler()));
        }

        template <class Function, class Handler, class Arg1, class Arg2>
        void asio_handler_invoke(Function&& function, binder2<Handler, Arg1, Arg2>* const this_handler)
        {
            using boost::asio::asio_handler_invoke;
            asio_handler_invoke(std::forward<Function>(function), std::addressof(this_handler->handler()));
        }

    } // namespace bind_handler_detail

    template <class Handler, class Arg1>
    auto bind(Handler&& handler, Arg1&& arg1)
        -> bind_handler_detail::binder1<
              typename canard::remove_cv_and_reference<Handler>::type
            , typename canard::remove_cv_and_reference<Arg1>::type
           >
    {
        return bind_handler_detail::binder1<
              typename canard::remove_cv_and_reference<Handler>::type
            , typename canard::remove_cv_and_reference<Arg1>::type
        >{std::forward<Handler>(handler), std::forward<Arg1>(arg1)};
    }

    template <class Handler, class Arg1, class Arg2>
    auto bind(Handler&& handler, Arg1&& arg1, Arg2&& arg2)
        -> bind_handler_detail::binder2<
              typename canard::remove_cv_and_reference<Handler>::type
            , typename canard::remove_cv_and_reference<Arg1>::type
            , typename canard::remove_cv_and_reference<Arg2>::type
           >
    {
        return bind_handler_detail::binder2<
              typename canard::remove_cv_and_reference<Handler>::type
            , typename canard::remove_cv_and_reference<Arg1>::type
            , typename canard::remove_cv_and_reference<Arg2>::type
        >{std::forward<Handler>(handler), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)};
    }

} // namespace detail
} // namespace canard

#endif // CANARD_ASIO_BIND_HANDLER_HPP
