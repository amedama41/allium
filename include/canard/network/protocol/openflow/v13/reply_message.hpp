#ifndef CANARD_NETWORK_OPENFLOW_V13_REPLY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_REPLY_HPP

#include <memory>
#include <utility>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/handler_continuation_hook.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/optional/optional.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/request_to_reply.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class Timer = boost::asio::steady_timer>
        class reply_message_base
        {
        private:
            template <class WaitHandler>
            using wait_result = canard::async_result_init<
                  typename canard::remove_cv_and_reference<WaitHandler>::type
                , void(boost::system::error_code)
            >;

        public:
            using time_point = typename Timer::time_point;
            using duration = typename Timer::duration;

        protected:
            reply_message_base(boost::asio::io_service& io_service
                    , ofp_type const request_type, ofp_type const reply_type)
                : timer_{io_service}
                , error_{}
                , request_type_(request_type)
                , reply_type_(reply_type)
            {
            }

        public:
            auto error() const
                -> boost::system::error_code
            {
                return error_;
            }

            void error(boost::system::error_code error)
            {
                error_ = error;
                timer_.cancel();
            }

            auto request_type() const
                -> ofp_type
            {
                return request_type_;
            }

            auto reply_type() const
                -> ofp_type
            {
                return reply_type_;
            }

            auto cancel()
                -> std::size_t
            {
                return timer_.cancel();
            }

            auto cancel(boost::system::error_code& ec)
                -> std::size_t
            {
                return timer_.cancel(ec);
            }

            auto cancel_one()
                -> std::size_t
            {
                return timer_.cancel_one();
            }

            auto cancel_one(boost::system::error_code& ec)
                -> std::size_t
            {
                return timer_.cancel_one(ec);
            }

            auto expires_at() const
                -> time_point
            {
                return timer_.expires_at();
            }

            auto expires_from_now() const
                -> duration
            {
                return timer_.expires_from_now();
            }

        protected:
            auto expires_at(time_point const& expire_time)
                -> std::size_t
            {
                return timer_.expires_at(expire_time);
            }

            auto expires_at(time_point const& expire_time, boost::system::error_code& ec)
                -> std::size_t
            {
                return timer_.expires_at(expire_time, ec);
            }

            auto expires_from_now(duration const& expiry_time)
                -> std::size_t
            {
                return timer_.expires_from_now(expiry_time);
            }

            auto expires_from_now(duration const& expiry_time, boost::system::error_code& ec)
                -> std::size_t
            {
                return timer_.expires_from_now(expiry_time, ec);
            }

            template <class WaitHandler>
            auto async_wait(WaitHandler&& handler)
                -> typename wait_result<WaitHandler>::result_type
            {
                return timer_.async_wait(std::forward<WaitHandler>(handler));
            }

        private:
            Timer timer_;
            boost::system::error_code error_;
            ofp_type request_type_;
            ofp_type reply_type_;
        };

        template <class ReplyMessage, class ReplyWaitHandler>
        class reply_wait_op
        {
        public:
            reply_wait_op(std::shared_ptr<ReplyMessage> message, ReplyWaitHandler handler)
                : message_(std::move(message))
                , handler_(std::move(handler))
            {
            }

            void operator()(boost::system::error_code const& error)
            {
                if (message_->error()) {
                    handler_(message_->error(), message_->message());
                }
                else {
                    handler_(message_->message() ? boost::system::error_code{} : error, message_->message());
                }
            }

            friend auto asio_handler_allocate(std::size_t const size, reply_wait_op* const context)
                -> void*
            {
                using boost::asio::asio_handler_allocate;
                return asio_handler_allocate(size, std::addressof(context->handler_));
            }

            friend void asio_handler_deallocate(void* const pointer, std::size_t const size, reply_wait_op* const context)
            {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(pointer, size, std::addressof(context->handler_));
            }

            template <class Function>
            friend void asio_handler_invoke(Function&& function, reply_wait_op* const context)
            {
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(std::forward<Function>(function), std::addressof(context->handler_));
            }

            friend auto asio_handler_is_continuation(reply_wait_op* const context)
                -> bool
            {
                using boost::asio::asio_handler_is_continuation;
                return asio_handler_is_continuation(std::addressof(context->handler_));
            }

        private:
            std::shared_ptr<ReplyMessage> message_;
            ReplyWaitHandler handler_;
        };

    } // namespace detail


    template <class T, class Timer = boost::asio::steady_timer>
    class reply_message
        : public detail::reply_message_base<Timer>
        , public std::enable_shared_from_this<reply_message<T, Timer>>
    {
        template <class ReplyWaitHandler>
        using reply_wait_result = canard::async_result_init<
              typename canard::remove_cv_and_reference<ReplyWaitHandler>::type
            , void(boost::system::error_code, boost::optional<T>)
        >;

    public:
        using time_point = typename Timer::time_point;
        using duration = typename Timer::duration;

        template <class RequestType>
        reply_message(boost::asio::io_service& io_service, RequestType*)
            : detail::reply_message_base<Timer>{io_service, RequestType::message_type, T::message_type}
            , reply_message_{boost::none}
        {
            static_assert(std::is_same<typename request_to_reply<RequestType>::type, T>::value
                    , "RequestType must be same as request type of T");
        }

        ~reply_message()
        {
            std::cout << __func__ << std::endl;
        }

        auto message()
            -> boost::optional<T>&
        {
            return reply_message_;
        }

        auto message() const
            -> boost::optional<T> const&
        {
            return reply_message_;
        }

        template <class MessageType>
        void message(MessageType&& reply_message)
        {
            reply_message_ = std::forward<MessageType>(reply_message);
            this->cancel();
        }

        template <class ReplyWaitHandler>
        auto async_wait_at(time_point const& expire_time, ReplyWaitHandler&& handler)
            -> typename reply_wait_result<ReplyWaitHandler>::result_type
        {
            this->expires_at(expire_time);
            return async_wait_impl(std::forward<ReplyWaitHandler>(handler));
        }

        template <class ReplyWaitHandler>
        auto async_wait_for(duration const& expire_time, ReplyWaitHandler&& handler)
            -> typename reply_wait_result<ReplyWaitHandler>::result_type
        {
            this->expires_from_now(expire_time);
            return async_wait_impl(std::forward<ReplyWaitHandler>(handler));
        }

    private:
        template <class ReplyWaitHandler>
        auto async_wait_impl(ReplyWaitHandler&& handler)
            -> typename reply_wait_result<ReplyWaitHandler>::result_type
        {
            using handler_type = typename reply_wait_result<ReplyWaitHandler>::handler_type;

            auto result = reply_wait_result<ReplyWaitHandler>{std::forward<ReplyWaitHandler>(handler)};
            this->async_wait(detail::reply_wait_op<reply_message, handler_type>{this->shared_from_this(), result.handler()});

            return result.get();
        }

    private:
        boost::optional<T> reply_message_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_REPLY_HPP
