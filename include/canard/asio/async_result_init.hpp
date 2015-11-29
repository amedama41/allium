#ifndef CANARD_ASIO_ASYNC_RESULT_INIT_HPP
#define CANARD_ASIO_ASYNC_RESULT_INIT_HPP

#include <utility>
#include <boost/asio/async_result.hpp>
#include <boost/asio/handler_type.hpp>

namespace canard {

    template <class Handler, class Signature>
    class async_result_init
    {
    public:
        using handler_type = typename boost::asio::handler_type<Handler, Signature>::type;
        using result_type = typename boost::asio::async_result<handler_type>::type;

        explicit async_result_init(Handler& handler)
            : handler_(handler)
            , result_{handler_}
        {
        }

        explicit async_result_init(Handler&& handler)
            : handler_(std::move(handler))
            , result_{handler_}
        {
        }

        auto handler()
            -> handler_type&
        {
            return handler_;
        }

        auto get()
            -> result_type
        {
            return result_.get();
        }

    private:
        handler_type handler_;
        boost::asio::async_result<handler_type> result_;
    };

} // namespace canard

#endif // CANARD_ASIO_ASYNC_RESULT_INIT_HPP
