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
        class transaction_base
        {
        private:
            template <class WaitHandler>
            using wait_handler_result_init = canard::async_result_init<
                  typename canard::remove_cv_and_reference<WaitHandler>::type
                , void(boost::system::error_code)
            >;

        public:
            using time_point = typename Timer::time_point;
            using duration = typename Timer::duration;

        protected:
            transaction_base(boost::asio::io_service& io_service
                    , ofp_type const request_type, ofp_type const reply_type)
                : timer_{io_service}
                , error_{}
                , request_type_(request_type)
                , reply_type_(reply_type)
            {
            }

        public:
            auto error() const
                -> boost::system::error_code const&
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

        protected:
            template <class WaitHandler>
            auto async_wait(WaitHandler&& handler)
                -> typename wait_handler_result_init<WaitHandler>::result_type
            {
                return timer_.async_wait(std::forward<WaitHandler>(handler));
            }

        private:
            Timer timer_;
            boost::system::error_code error_;
            ofp_type request_type_;
            ofp_type reply_type_;
        };

        template <class Transaction, class WaitForReplyHandler>
        class wait_handler_adaptor
        {
        public:
            wait_handler_adaptor(std::shared_ptr<Transaction> transaction, WaitForReplyHandler handler)
                : transaction_(std::move(transaction))
                , handler_(std::move(handler))
            {
            }

            void operator()(boost::system::error_code const& ec)
            {
                if (transaction_->error()) {
                    handler_(transaction_->error(), transaction_->reply());
                }
                else {
                    handler_(transaction_->reply() ? boost::system::error_code{} : ec, transaction_->reply());
                }
            }

            friend auto asio_handler_allocate(std::size_t const size, wait_handler_adaptor* const context)
                -> void*
            {
                using boost::asio::asio_handler_allocate;
                return asio_handler_allocate(size, std::addressof(context->handler_));
            }

            friend void asio_handler_deallocate(void* const pointer, std::size_t const size, wait_handler_adaptor* const context)
            {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(pointer, size, std::addressof(context->handler_));
            }

            template <class Function>
            friend void asio_handler_invoke(Function&& function, wait_handler_adaptor* const context)
            {
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(std::forward<Function>(function), std::addressof(context->handler_));
            }

            friend auto asio_handler_is_continuation(wait_handler_adaptor* const context)
                -> bool
            {
                using boost::asio::asio_handler_is_continuation;
                return asio_handler_is_continuation(std::addressof(context->handler_));
            }

        private:
            std::shared_ptr<Transaction> transaction_;
            WaitForReplyHandler handler_;
        };

        template <class Transaction, class WaitForReplyHandler>
        inline auto make_wait_handler_adaptor(std::shared_ptr<Transaction> trans, WaitForReplyHandler&& handler)
            -> wait_handler_adaptor<
                  Transaction
                , typename canard::remove_cv_and_reference<WaitForReplyHandler>::type>
        {
            return wait_handler_adaptor<
                  Transaction
                , typename canard::remove_cv_and_reference<WaitForReplyHandler>::type>{
                std::move(trans), std::forward<WaitForReplyHandler>(handler)
            };
        }

    } // namespace detail


    template <class Message, class Timer = boost::asio::steady_timer>
    class transaction_impl
        : public detail::transaction_base<Timer>
        , public std::enable_shared_from_this<transaction_impl<Message, Timer>>
    {
        using base_type = detail::transaction_base<Timer>;

        template <class WaitForReplyHandler>
        using async_wait_for_reply_result_init = canard::async_result_init<
              typename canard::remove_cv_and_reference<WaitForReplyHandler>::type
            , void(boost::system::error_code, boost::optional<Message>)
        >;

    public:
        using time_point = typename Timer::time_point;
        using duration = typename Timer::duration;

        template <class RequestType>
        transaction_impl(boost::asio::io_service& io_service, RequestType*)
            : base_type{io_service, RequestType::message_type, Message::message_type}
            , reply_{boost::none}
        {
            static_assert(std::is_same<typename request_to_reply<RequestType>::type, Message>::value
                    , "RequestType must be same as request type of Message");
        }

        auto reply()
            -> boost::optional<Message>&
        {
            return reply_;
        }

        auto reply() const
            -> boost::optional<Message> const&
        {
            return reply_;
        }

        void reply(Message const& reply)
        {
            reply_ = reply;
            this->cancel();
        }

        void reply(Message&& reply)
        {
            reply_ = std::forward<Message>(reply);
            this->cancel();
        }

        template <class WaitForReplyHandler>
        auto async_wait(WaitForReplyHandler&& handler)
            -> typename async_wait_for_reply_result_init<WaitForReplyHandler>::result_type
        {
            auto result = async_wait_for_reply_result_init<WaitForReplyHandler>{
                std::forward<WaitForReplyHandler>(handler)
            };

            base_type::async_wait(detail::make_wait_handler_adaptor(
                    this->shared_from_this(), std::move(result.handler())
            ));

            return result.get();
        }

    private:
        boost::optional<Message> reply_;
    };

    template <class Message, class Timer = boost::asio::steady_timer>
    class transaction
    {
        template <class WaitForReplyHandler>
        using async_wait_for_reply_result_init = canard::async_result_init<
              typename canard::remove_cv_and_reference<WaitForReplyHandler>::type
            , void(boost::system::error_code, boost::optional<Message>)
        >;

    public:
        using time_point = typename Timer::time_point;
        using duration = typename Timer::duration;

        explicit transaction(std::shared_ptr<transaction_impl<Message, Timer>> pimpl)
            : pimpl_(std::move(pimpl))
        {
        }

        auto cancel()
            -> std::size_t
        {
            return pimpl_->cancel();
        }

        auto cancel(boost::system::error_code& ec)
            -> std::size_t
        {
            return pimpl_->cancel(ec);
        }

        auto cancel_one()
            -> std::size_t
        {
            return pimpl_->cancel_one();
        }

        auto cancel_one(boost::system::error_code& ec)
            -> std::size_t
        {
            return pimpl_->cancel_one(ec);
        }

        auto expires_at() const
            -> time_point
        {
            return pimpl_->expires_at();
        }

        auto expires_from_now() const
            -> duration
        {
            return pimpl_->expires_from_now();
        }

        auto expires_at(time_point const& expire_time)
            -> std::size_t
        {
            return pimpl_->expires_at(expire_time);
        }

        auto expires_at(time_point const& expire_time, boost::system::error_code& ec)
            -> std::size_t
        {
            return pimpl_->expires_at(expire_time, ec);
        }

        auto expires_from_now(duration const& expiry_time)
            -> std::size_t
        {
            return pimpl_->expires_from_now(expiry_time);
        }

        auto expires_from_now(duration const& expiry_time, boost::system::error_code& ec)
            -> std::size_t
        {
            return pimpl_->expires_from_now(expiry_time, ec);
        }

        template <class WaitForReplyHandler>
        auto async_wait(WaitForReplyHandler&& handler)
            -> typename async_wait_for_reply_result_init<WaitForReplyHandler>::result_type
        {
            return pimpl_->async_wait(std::forward<WaitForReplyHandler>(handler));
        }

    private:
        std::shared_ptr<transaction_impl<Message, Timer>> pimpl_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_REPLY_HPP
