#ifndef CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional/optional.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/ordered_send_socket.hpp>
#include <canard/network/protocol/openflow/v13/detail/dummy_handler.hpp>
#include <canard/network/protocol/openflow/v13/detail/shared_buffer_handler.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>
#include <canard/network/protocol/openflow/v13/transaction.hpp>
#include <canard/network/protocol/openflow/v13/request_to_reply.hpp>
#include <canard/network/utils/thread_pool.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    template <class Socket = boost::asio::ip::tcp::socket>
    class openflow_channel
        : public std::enable_shared_from_this<openflow_channel<Socket>>
    {
        using endpoint_type = typename Socket::lowest_layer_type::endpoint_type;

        using write_handler_type = void(boost::system::error_code, std::size_t);

        template <class Handler>
        using send_async_result = canard::async_result_init<
                canard::remove_cv_and_reference_t<Handler>, write_handler_type
        >;

        template <class Request>
        using reply_t = typename request_to_reply<Request>::type;

        template <class Request>
        using request_handler_type = void(boost::system::error_code, transaction<reply_t<Request>>);

        template <class Request, class RequestHandler>
        using request_async_result = canard::async_result_init<
                canard::remove_cv_and_reference_t<RequestHandler>, request_handler_type<Request>
        >;

        using close_handler_type = void(*)(openflow_channel*, boost::system::error_code);

    public:
        openflow_channel(boost::asio::io_service& io_service
                , utils::thread_pool& thread_pool
                , close_handler_type close_handler)
            : stream_{io_service}
            , strand_{io_service}
            , endpoint_{boost::none}
            , thread_pool_(thread_pool)
            , close_handler_(close_handler)
        {
        }

        auto endpoint() const
            -> endpoint_type const&
        {
            return *endpoint_;
        }

        auto thread_pool()
            -> utils::thread_pool&
        {
            return thread_pool_;
        }

        template <class Message, class Handler, class Container>
        auto send(Message const& message, Handler&& handler, Container& buffer)
            -> typename send_async_result<Handler>::result_type
        {
            auto init = send_async_result<Handler>{std::forward<Handler>(handler)};
            send_impl(message, std::move(init.handler()), buffer);
            return init.get();
        }

        template <class Message, class Handler>
        auto send(Message const& message, Handler&& handler)
            -> typename send_async_result<Handler>::result_type
        {
            auto buffer_handler = detail::make_shared_buffer_handler(
                    typename send_async_result<Handler>::handler_type(std::forward<Handler>(handler)));
            return send(message, buffer_handler, buffer_handler.buffer());
        }

        template <class Message>
        auto send(Message const& message)
            -> typename send_async_result<detail::dummy_handler>::result_type
        {
            return send(message, detail::dummy_handler{});
        }

        template <class Request, class RequestHandler, class Container>
        auto send_request(Request const& message, RequestHandler&& handler, Container& buffer)
            -> typename request_async_result<Request, RequestHandler>::result_type
        {
            auto init = request_async_result<Request, RequestHandler>{std::forward<RequestHandler>(handler)};
            buffer.clear();
            strand_.dispatch(request_sender<Request, canard::remove_cv_and_reference_t<RequestHandler>, Container>{
                message, std::move(init.handler()), buffer, this->shared_from_this()
            });

            return init.get();
        }

        template <class Request, class RequestHandler>
        auto send_request(Request const& message, RequestHandler&& handler)
            -> typename request_async_result<Request, RequestHandler>::result_type
        {
            auto buffer_handler = detail::make_shared_buffer_handler(
                    typename request_async_result<Request, RequestHandler>::handler_type(
                        std::forward<RequestHandler>(handler)));
            return send_request(message, buffer_handler, buffer_handler.buffer());
        }

        void close(boost::system::error_code const& ec)
        {
            close_handler_(this, ec);
        }

    protected:
        template <class ConstBufferSequence, class Handler>
        void async_send(ConstBufferSequence const& buffers, Handler&& handler)
        {
            using handler_type = canard::remove_cv_and_reference_t<Handler>;
            stream_.async_write_some(buffers
                    , strand_.wrap(send_handler_adaptor<handler_type>{
                        this->shared_from_this(), std::forward<Handler>(handler)
            }));
        }

    private:
        template <class Reply>
        class transaction_deleter
        {
        public:
            explicit transaction_deleter(std::shared_ptr<openflow_channel> channel, std::uint32_t const xid)
                : channel_(std::move(channel))
                , xid_(xid)
            {
            }

            void operator()(transaction_impl<Reply>* const txn) const
            {
                channel_->strand_.dispatch([=](){
                    channel_->reply_map_.erase(xid_);
                    delete txn;
                });
            }

        private:
            std::shared_ptr<openflow_channel> channel_;
            std::uint32_t xid_;
        };

        template <class Message, class RequestHandler, class Container>
        class request_sender
        {
            using reply_type = typename request_to_reply<Message>::type;
            using transaction_type = transaction_impl<reply_type>;
            using transaction_ptr = std::shared_ptr<transaction_type>;

        public:
            request_sender(Message const& message, RequestHandler handler
                    , Container& buffer, std::shared_ptr<openflow_channel> channel)
                : handler_(std::move(handler))
                , xid_(message.xid())
                , buffer_(message.encode(buffer))
                , channel_(std::move(channel))
            {
            }

            void operator()(boost::system::error_code const& ec, std::size_t) {
                channel_->thread_pool_.post(canard::detail::bind(
                          std::move(handler_), ec, transaction<reply_type>{std::move(transaction_)}));
            }

            void operator()() {
                transaction_ = channel_->template register_request<Message>(xid_);
                channel_->async_send(boost::asio::buffer(buffer_), std::move(*this));
            }

        private:
            RequestHandler handler_;
            std::uint64_t xid_;
            Container& buffer_;
            std::shared_ptr<openflow_channel> channel_;
            std::shared_ptr<transaction_type> transaction_;
        };

        template <class WriteHandler>
        class send_handler_adaptor
        {
        public:
            send_handler_adaptor(std::shared_ptr<openflow_channel> channel, WriteHandler handler)
                : channel_(std::move(channel)), handler_(std::move(handler))
            {
            }

            void operator()(boost::system::error_code const& ec, std::size_t const bytes_transferred)
            {
                auto function = canard::detail::bind(std::move(handler_), ec, bytes_transferred);
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
                if (ec) {
                    channel_->close(ec);
                }
            }

        private:
            std::shared_ptr<openflow_channel> channel_;
            WriteHandler handler_;
        };


        template <class Message, class WriteHandler, class Container>
        void send_impl(Message const& message, WriteHandler handler, Container& buffer)
        {
            buffer.clear();
            message.encode(buffer);
            strand_.dispatch([&, handler]() mutable {
                async_send(boost::asio::buffer(buffer), [&, handler](boost::system::error_code ec, std::size_t bytes_transferred) mutable {
                    thread_pool_.post(canard::detail::bind(std::move(handler), ec, bytes_transferred));
                });
            });
        }

        template <class Request>
        auto register_request(std::uint32_t const xid)
            -> std::shared_ptr<transaction_impl<reply_t<Request>>>
        {
            using reply_type = reply_t<Request>;
            auto reply = std::shared_ptr<transaction_impl<reply_type>>(
                  new transaction_impl<reply_type>{stream_.next_layer().get_io_service(), static_cast<Request*>(nullptr)}
                , transaction_deleter<reply_type>{this->shared_from_this(), xid});
            reply_map_.emplace(xid, reply.get());
            return reply;
        }

    protected:
        canard::ordered_send_socket<Socket> stream_;
        boost::asio::io_service::strand strand_;
        std::unordered_map<std::uint32_t, v13_detail::transaction_base<>*> reply_map_;
        boost::optional<endpoint_type> endpoint_;
        utils::thread_pool& thread_pool_;
        close_handler_type close_handler_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
