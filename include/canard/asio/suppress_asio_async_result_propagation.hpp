#ifndef CANARD_ASIO_SUPPRESS_ASIO_ASYNC_RESULT_PROPAGATION_HPP
#define CANARD_ASIO_SUPPRESS_ASIO_ASYNC_RESULT_PROPAGATION_HPP

#include <utility>
#include <type_traits>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/handler_continuation_hook.hpp>
#include <boost/asio/handler_invoke_hook.hpp>

namespace canard {

  namespace detail {

    template <class Handler>
    class no_asio_async_result_propagation
    {
    public:
      explicit no_asio_async_result_propagation(Handler& handler)
        : handler_(handler)
      {
      }

      explicit no_asio_async_result_propagation(Handler&& handler)
        : handler_(std::move(handler))
      {
      }

      template <class... Args>
      auto operator()(Args&&... args)
        -> decltype(std::declval<Handler>()(std::forward<Args>(args)...))
      {
        handler_(std::forward<Args>(args)...);
      }

      template <class... Args>
      auto operator()(Args&&... args) const
        -> decltype(std::declval<Handler const>()(std::forward<Args>(args)...))
      {
        handler_(std::forward<Args>(args)...);
      }

      auto handler()
        -> Handler&
      {
        return handler_;
      }

    private:
      Handler handler_;
    };

    template <class Handler>
    auto asio_handler_allocate(
          std::size_t const size
        , no_asio_async_result_propagation<Handler>* const this_handler)
      -> void*
    {
      using boost::asio::asio_handler_allocate;
      return asio_handler_allocate(
          size, std::addressof(this_handler->handler()));
    }

    template <class Handler>
    void asio_handler_deallocate(
          void* const pointer, std::size_t const size
        , no_asio_async_result_propagation<Handler>* const this_handler)
    {
      using boost::asio::asio_handler_deallocate;
      asio_handler_deallocate(
          pointer, size, std::addressof(this_handler->handler()));
    }

    template <class Handler>
    auto asio_handler_is_continuation(
        no_asio_async_result_propagation<Handler>* const this_handler)
      -> bool
    {
      using boost::asio::asio_handler_is_continuation;
      return asio_handler_is_continuation(
          std::addressof(this_handler->handler()));
    }

    template <class Function, class Handler>
    void asio_handler_invoke(
          Function&& function
        , no_asio_async_result_propagation<Handler>* const this_handler)
    {
      using boost::asio::asio_handler_invoke;
      asio_handler_invoke(
            std::forward<Function>(function)
          , std::addressof(this_handler->handler()));
    }

  } // namespace detail

  template <class Handler, class T = typename std::decay<Handler>::type>
  auto suppress_asio_async_result_propagation(Handler&& handler)
    -> detail::no_asio_async_result_propagation<T>
  {
    return detail::no_asio_async_result_propagation<T>{
      std::forward<Handler>(handler)
    };
  }

} // namespace canard

#endif // CANARD_ASIO_SUPPRESS_ASIO_ASYNC_RESULT_PROPAGATION_HPP
