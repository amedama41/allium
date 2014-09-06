#ifndef CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP

#include <cstddef>
#include <chrono>
#include <iterator>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/handler_type.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/optional/optional.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/ordered_send_socket.hpp>
#include <canard/network/protocol/openflow/v13/detail/dummy_handler.hpp>
#include <canard/network/protocol/openflow/v13/detail/request_handler.hpp>
#include <canard/network/protocol/openflow/v13/detail/shared_buffer_handler.hpp>
#include <canard/network/protocol/openflow/v13/disconnected_info.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>
#include <canard/network/protocol/openflow/v13/reply_message.hpp>
#include <canard/network/protocol/openflow/v13/request_to_reply.hpp>
#include <canard/network/protocol/openflow/v13/to_error_code.hpp>
#include <canard/network/utils/thread_pool.hpp>
#include <canard/type_traits.hpp>

#include <iostream>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        inline auto parse_header(boost::asio::const_buffer const& buffer)
            -> detail::ofp_header
        {
            auto header = detail::ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<unsigned char const*>(buffer)
                      , sizeof(detail::ofp_header));
            return detail::ntoh(header);
        }

        template <class T>
        auto is_null_decorator(null_decorator::type<T>*)
            -> std::true_type;
        auto is_null_decorator(...)
            -> std::false_type;

        template <class T>
        struct is_derived_of_null_decorator
            : decltype(is_null_decorator(static_cast<T*>(nullptr)))
        {};

        template <class T>
        inline auto get_base_type(null_decorator::type<T>& controller)
            -> null_decorator::type<T>&
        {
            return controller;
        }

        template <class T, typename std::enable_if<!is_derived_of_null_decorator<T>::value>::type* = nullptr>
        inline auto get_base_type(T& controller)
            -> T&
        {
            return controller;
        }

    } // namespace detail

    template <class ControllerHandler, class Socket = boost::asio::ip::tcp::socket>
    class openflow_channel
        : public std::enable_shared_from_this<openflow_channel<ControllerHandler>>
    {
        using endpoint_type = typename Socket::lowest_layer_type::endpoint_type;
        template <class Handler>
        using send_handler_type = typename boost::asio::handler_type<
                  typename canard::remove_cv_and_reference<Handler>::type
                , void(boost::system::error_code, std::size_t)
        >::type;
        template <class Handler, class T>
        using request_handler_type = typename boost::asio::handler_type<
                  typename canard::remove_cv_and_reference<Handler>::type
                , void(boost::system::error_code, std::shared_ptr<reply_message<typename request_to_reply<T>::type>>)
        >::type;

    public:
        openflow_channel(boost::asio::io_service& io_service
                , ControllerHandler& controller_handler
                , utils::thread_pool& thread_pool)
            : stream_{io_service}
            , strand_{io_service}
            , streambuf_{}
            , timer_{io_service}
            , endpoint_{boost::none}
            , controller_handler_(controller_handler)
            , thread_pool_(thread_pool)
        {
        }

        auto socket()
            -> Socket&
        {
            return stream_.next_layer();
        }

        void run()
        {
            send_buffer_.clear();
            auto this_ = this->shared_from_this();
            async_send(boost::asio::buffer(hello{}.encode(send_buffer_))
                    , [this_](boost::system::error_code const& error, std::size_t) {
                if (not error) {
                    this_->start_message_loop();
                }
            });
        }

        auto endpoint() const
            -> endpoint_type const&
        {
            return *endpoint_;
        }

        template <class T, class Handler, class Container>
        auto send(T const& message, Handler&& handler, Container& buffer)
            -> typename boost::asio::async_result<send_handler_type<Handler>>::type
        {
            auto init = canard::async_result_init<
                typename canard::remove_cv_and_reference<Handler>::type, void(boost::system::error_code, std::size_t)
            >{std::forward<Handler>(handler)};
            send_impl(message, std::move(init.handler()), buffer);
            return init.get();
        }

        template <class T, class Handler>
        auto send(T const& message, Handler&& handler)
            -> typename boost::asio::async_result<send_handler_type<Handler>>::type
        {
            auto buffer_handler = detail::make_shared_buffer_handler(
                    send_handler_type<Handler>(std::forward<Handler>(handler)));
            return send(message, buffer_handler, buffer_handler.buffer());
        }

        template <class T>
        auto send(T const& message)
            -> typename boost::asio::async_result<send_handler_type<detail::dummy_handler>>::type
        {
            return send(message, detail::dummy_handler{});
        }

        template <class T, class RequestHandler, class Container>
        auto send_request(T const& message, RequestHandler&& handler, Container& buffer)
            -> typename boost::asio::async_result<request_handler_type<RequestHandler, T>>::type
        {
            auto req_handler = request_handler_type<RequestHandler, T>(std::forward<RequestHandler>(handler));
            auto result = boost::asio::async_result<request_handler_type<RequestHandler, T>>{req_handler};
            buffer.clear();
            message.encode(buffer);
            auto this_ = this->shared_from_this();
            auto xid = message.xid();
            strand_.dispatch([&, this_, xid, req_handler]() mutable {
                auto reply = this_->template register_request<T>(xid);
                async_send(boost::asio::buffer(buffer), [&, req_handler, reply](boost::system::error_code const& ec, std::size_t) {
                    thread_pool_.post(canard::detail::bind(std::move(req_handler), ec, std::move(reply)));
                });
            });
            return result.get();
        }

        template <class T, class RequestHandler>
        auto send_request(T const& message, RequestHandler&& handler)
            -> typename boost::asio::async_result<request_handler_type<RequestHandler, T>>::type
        {
            auto buffer_handler = detail::make_shared_buffer_handler(
                    request_handler_type<RequestHandler, T>(std::forward<RequestHandler>(handler)));
            return send_request(message, buffer_handler, buffer_handler.buffer());
        }

    private:
        template <class T>
        auto register_request(std::uint32_t const xid)
            -> std::shared_ptr<reply_message<typename request_to_reply<T>::type>>
        {
            using reply_type = typename request_to_reply<T>::type;
            auto this_ = this->shared_from_this();
            auto reply = std::shared_ptr<reply_message<reply_type>>(
                  new reply_message<reply_type>{socket().get_io_service(), static_cast<T*>(nullptr)}
                , [this_, xid](reply_message<reply_type>* const p) mutable {
                    auto& channel = *this_;
                    channel.strand_.dispatch([=]() {
                        this_->reply_map_.erase(xid);
                        delete p;
                    });
                }
            );
            reply_map_.emplace(xid, reply.get());
            return reply;
        }

        template <class T, class WriteHandler, class Container>
        void send_impl(T const& message, WriteHandler handler, Container& buffer)
        {
            buffer.clear();
            message.encode(buffer);
            strand_.dispatch([&, handler]() mutable {
                async_send(boost::asio::buffer(buffer), [&, handler](boost::system::error_code ec, std::size_t bytes_transferred) mutable {
                    thread_pool_.post(canard::detail::bind(std::move(handler), ec, bytes_transferred));
                });
            });
        }

        template <class ConstBufferSequence, class Handler>
        void async_send(ConstBufferSequence const& buffers, Handler handler)
        {
            auto this_ = this->shared_from_this();
            stream_.async_write_some(buffers
                    , strand_.wrap([this_, handler](boost::system::error_code const& error, std::size_t const bytes_transferred) mutable {
                if (error) {
                    this_->socket().close();
                    this_->timer_.cancel();
                    this_->handle_disconnected(error);
                    return;
                }
                auto function = canard::detail::bind(std::move(handler), error, bytes_transferred);
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
            }));
        }

        void start_message_loop()
        {
            receive_messages(sizeof(detail::ofp_header));
        }

        void receive_messages(std::size_t const least_length)
        {
            auto this_ = this->shared_from_this();
            boost::asio::async_read(stream_, streambuf_, boost::asio::transfer_at_least(least_length)
                    , strand_.wrap([this_](boost::system::error_code const& error, std::size_t const bytes_transferred) {
                if (error) {
                    this_->socket().close();
                    this_->timer_.cancel();
                    this_->handle_disconnected(error);
                    return;
                }
                this_->set_echo_request_timer();
                auto const next_reading_size = this_->handle_messages();
                this_->receive_messages(next_reading_size);
            }));
        }

        auto handle_messages()
            -> std::size_t
        {
            while (streambuf_.size() >= sizeof(detail::ofp_header)) {
                auto const header = detail::parse_header(streambuf_.data());
                if (streambuf_.size() < header.length) {
                    return header.length - streambuf_.size();
                }
                handle_message(header);
            }
            return sizeof(detail::ofp_header) - streambuf_.size();
        }

        void set_echo_request_timer()
        {
            auto this_ = this->shared_from_this();
            timer_.expires_from_now(std::chrono::seconds{30});
            timer_.async_wait(strand_.wrap([this_](boost::system::error_code const& error) {
                if (not error) {
                    this_->send_echo_request();
                }
            }));
        }

        void send_echo_request()
        {
            std::cout << __func__ << std::endl;
            auto this_ = this->shared_from_this();
            send_buffer_.clear();
            async_send(boost::asio::buffer(echo_request{}.encode(send_buffer_)), [](boost::system::error_code, std::size_t){});
            timer_.expires_from_now(std::chrono::seconds{10});
            timer_.async_wait(strand_.wrap([this_](boost::system::error_code const& error) {
                if (not error) {
                    std::cout << "no response from switch" << std::endl;
                    this_->socket().close();
                    this_->timer_.cancel();
                }
            }));
        }

        /*****************************************************************************************/

        void handle_message(detail::ofp_header const& header);

        void handle_disconnected(boost::system::error_code const& error)
        {
            if (endpoint_) {
                auto this_ = this->shared_from_this();
                thread_pool_.post([=]() {
                    detail::get_base_type(this_->controller_handler_).handle(this_, disconnected_info{error});
                });
                endpoint_ = boost::none;
            }
        }

        void handle_hello(detail::ofp_header const& header)
        {
            auto const hello_msg = decode<hello>(header);
            std::cout << __func__ << ": " << hello_msg << std::endl;

            if (endpoint_) {
                std::cerr << "received multiple-hello messages" << std::endl;
                return;
            }
            endpoint_ = socket().remote_endpoint();
            auto this_ = this->shared_from_this();
            thread_pool_.post([this_]() {
                    detail::get_base_type(this_->controller_handler_).handle(this_);
            });
        }

        void handle_error(detail::ofp_header const& header)
        {
            auto error = decode<error_msg>(header);
            auto const it = reply_map_.find(error.xid());
            if (it != reply_map_.end() && error.failed_request_header().type == it->second->request_type()) {
                auto reply = it->second;
                thread_pool_.post([=]() {
                    reply->error(to_error_code(error));
                });
                reply_map_.erase(it);
                return;
            }
            auto this_ = this->shared_from_this();
            thread_pool_.post([this_, error]() mutable {
                    detail::get_base_type(this_->controller_handler_).handle(this_, std::move(error));
            });
        }

        void handle_echo_request(detail::ofp_header const& header)
        {
            auto echo_req = decode<echo_request>(header);
            std::cout << __func__ << ": " << echo_req << std::endl;
            auto buffer_handler = detail::make_shared_buffer_handler([](boost::system::error_code, std::size_t){});
            async_send(boost::asio::buffer(std::move(echo_req).reply().encode(buffer_handler.buffer())), buffer_handler);
        }

        void handle_multipart();

        template <class T>
        void handle(detail::ofp_header const& header)
        {
            auto message = decode<T>(header);
            auto this_ = this->shared_from_this();
            thread_pool_.post([message, this_]() mutable {
                    detail::get_base_type(this_->controller_handler_).handle(this_, std::move(message));
            });
        }

        template <class T>
        void handle_reply(detail::ofp_header const& header)
        {
            auto message = decode<T>(header);
            auto const it = reply_map_.find(message.xid());
            if (it != reply_map_.end() && header.type == it->second->reply_type()) {
                auto reply = static_cast<reply_message<T>*>(it->second);
                thread_pool_.post([=]() mutable {
                    reply->message(std::move(message));
                });
                reply_map_.erase(it);
                return;
            }
            auto this_ = this->shared_from_this();
            thread_pool_.post([this_, message]() mutable {
                    detail::get_base_type(this_->controller_handler_).handle(this_, std::move(message));
            });
        }

    private:
        template <class T>
        auto decode(detail::ofp_header const& header)
            -> T
        {
            auto first = boost::asio::buffer_cast<unsigned char const*>(streambuf_.data());
            auto const last = std::next(first, header.length);
            auto message = T::decode(first, last);
            if (first != last) {
                std::cout << v13::to_string(ofp_type(header.type))
                    << ": message_error - " << std::distance(first, last) << std::endl;
            }
            streambuf_.consume(header.length);
            return message;
        }

    private:
        canard::ordered_send_socket<Socket> stream_;
        boost::asio::io_service::strand strand_;
        boost::asio::streambuf streambuf_;
        std::vector<unsigned char> send_buffer_;
        boost::asio::steady_timer timer_;
        std::unordered_map<std::uint32_t, detail::reply_message_base<>*> reply_map_;
        boost::optional<endpoint_type> endpoint_;
        ControllerHandler& controller_handler_;
        utils::thread_pool& thread_pool_;
    };

    template <class ControllerHandler, class Socket>
    inline void
    openflow_channel<ControllerHandler, Socket>::handle_multipart()
    {
        auto multipart_reply = detail::ofp_multipart_reply{};
        std::memcpy(&multipart_reply, boost::asio::buffer_cast<unsigned char const*>(streambuf_.data()), sizeof(multipart_reply));
        multipart_reply = ntoh(multipart_reply);
        using multipart_list = std::tuple<
              description_reply
            , flow_stats_reply
            , aggregate_stats_reply
            , table_stats_reply
            , port_stats_reply
            , table_features_reply
            , port_description_reply
        >;
        switch (multipart_reply.type) {
#       define CANARD_NETWORK_OPENFLOW_V13_MULTIPART_TYPE_APPLY_CASE(z, N, _) \
        case std::tuple_element<N, multipart_list>::type::multipart_type_value: \
            handle_reply<std::tuple_element<N, multipart_list>::type>(multipart_reply.header); \
            break;
        BOOST_PP_REPEAT(6, CANARD_NETWORK_OPENFLOW_V13_MULTIPART_TYPE_APPLY_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_MULTIPART_TYPE_APPLY_CASE

        default:
            [&]() {
                auto buf = std::vector<char>(multipart_reply.header.length);
                streambuf_.sgetn(&buf[0], buf.size());
                std::cout << "receive " << to_string(ofp_multipart_type(multipart_reply.type)) << std::endl;
            }();
        }
    }

    template <class ControllerHandler, class Socket>
    inline void
    openflow_channel<ControllerHandler, Socket>::handle_message(detail::ofp_header const& header)
    {
        if (!endpoint_ and header.type != OFPT_HELLO) {
            std::cerr << "received non-hello message before hello" << std::endl;
            socket().close();
            timer_.cancel();
            return;
        }

        using message_type_list = std::tuple<
            packet_in, flow_removed, port_status
        >;
        using reply_message_type_list = std::tuple<
            echo_reply, features_reply, switch_config_reply, barrier_reply
        >;
        switch (header.type) {
        case OFPT_HELLO:
            handle_hello(header);
            break;
        case OFPT_ERROR:
            handle_error(header);
            break;
        case OFPT_ECHO_REQUEST:
            handle_echo_request(header);
            break;
        case OFPT_MULTIPART_REPLY:
            handle_multipart();
            break;

#       define CANARD_NETWORK_OPENFLOW_V13_MESSAGE_TYPE_APPLY_CASE(z, N, _) \
        case std::tuple_element<N, message_type_list>::type::message_type: \
            handle<std::tuple_element<N, message_type_list>::type>(header); \
            break;
        BOOST_PP_REPEAT(3, CANARD_NETWORK_OPENFLOW_V13_MESSAGE_TYPE_APPLY_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_MESSAGE_TYPE_APPLY_CASE

#       define CANARD_NETWORK_OPENFLOW_V13_REPLY_MESSAGE_TYPE_APPLY_CASE(z, N, _) \
        case std::tuple_element<N, reply_message_type_list>::type::message_type: \
            handle_reply<std::tuple_element<N, reply_message_type_list>::type>(header); \
            break;
        BOOST_PP_REPEAT(3, CANARD_NETWORK_OPENFLOW_V13_REPLY_MESSAGE_TYPE_APPLY_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_REPLY_MESSAGE_TYPE_APPLY_CASE

        default:
            [&]() {
                auto buf = std::vector<char>(header.length);
                streambuf_.sgetn(&buf[0], buf.size());
                std::cout << "receive " << std::uint32_t(header.type) << std::endl;
            }();
        }
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
