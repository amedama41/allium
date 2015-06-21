#ifndef CANARD_NETWORK_OPENFLOW_V13_CHANNLE_IMPL_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CHANNLE_IMPL_HPP

#include <cstring>
#include <chrono>
#include <iterator>
#include <type_traits>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/disconnected_info.hpp>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include <canard/network/protocol/openflow/v13/to_error_code.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>

#include <iostream>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        inline auto parse_header(boost::asio::const_buffer const& buffer)
            -> v13_detail::ofp_header
        {
            auto header = v13_detail::ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<unsigned char const*>(buffer)
                      , sizeof(v13_detail::ofp_header));
            return v13_detail::ntoh(header);
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

    } // namespace v13_detail

    template <class ControllerHandler, class Socket = boost::asio::ip::tcp::socket>
    class openflow_channel_impl
        : public openflow_channel<Socket>
    {
        using base_type = openflow_channel<Socket>;

    public:
        openflow_channel_impl(boost::asio::io_service& io_service
                , ControllerHandler& controller_handler
                , utils::thread_pool& thread_pool)
            : base_type{io_service, thread_pool, &close_handler}
            , streambuf_{}
            , timer_{io_service}
            , controller_handler_(controller_handler)
        {
        }

        auto socket()
            -> Socket&
        {
            return this->stream_.next_layer();
        }

        void run()
        {
            send_buffer_.clear();
            auto this_ = shared_from_this();
            this->async_send(boost::asio::buffer(hello{}.encode(send_buffer_))
                    , [this_](boost::system::error_code const& error, std::size_t) {
                if (!error) {
                    this_->start_message_loop();
                }
            });
        }

    private:
        void handle_close(boost::system::error_code const& ec)
        {
            auto ignored = boost::system::error_code{};
            timer_.cancel(ignored);
            socket().close(ignored);
            handle_disconnected(ec);
        }

        static void close_handler(base_type* channel, boost::system::error_code ec)
        {
            static_cast<openflow_channel_impl*>(channel)->handle_close(ec);
        }

        class receive_messages_handler
        {
        public:
            explicit receive_messages_handler(std::shared_ptr<openflow_channel_impl> channel)
                : channel_(std::move(channel))
            {
            }

            void operator()(boost::system::error_code const& ec, std::size_t const) {
                if (ec) {
                    channel_->close(ec);
                    return;
                }
                channel_->set_echo_request_timer();
                auto const next_reading_size = channel_->handle_messages();
                auto const channel = channel_.get();
                channel->receive_messages(next_reading_size, std::move(*this));
            }

        private:
            std::shared_ptr<openflow_channel_impl> channel_;
        };

        void start_message_loop()
        {
            receive_messages(sizeof(v13_detail::ofp_header), receive_messages_handler{shared_from_this()});
        }

        void receive_messages(std::size_t const least_length, receive_messages_handler handler)
        {
            boost::asio::async_read(this->stream_, streambuf_
                    , boost::asio::transfer_at_least(least_length)
                    , this->strand_.wrap(std::move(handler)));
        }

        auto handle_messages()
            -> std::size_t
        {
            while (streambuf_.size() >= sizeof(v13_detail::ofp_header)) {
                auto const header = v13_detail::parse_header(streambuf_.data());
                if (streambuf_.size() < header.length) {
                    return header.length - streambuf_.size();
                }
                handle_message(header);
            }
            return sizeof(v13_detail::ofp_header) - streambuf_.size();
        }

        void set_echo_request_timer()
        {
            auto this_ = shared_from_this();
            timer_.expires_from_now(std::chrono::seconds{30});
            timer_.async_wait(this->strand_.wrap([this_](boost::system::error_code const& error) {
                if (!error) {
                    this_->send_echo_request();
                }
            }));
        }

        void send_echo_request()
        {
            std::cout << __func__;
            auto this_ = shared_from_this();
            send_buffer_.clear();
            this->async_send(boost::asio::buffer(echo_request{}.encode(send_buffer_)), [](boost::system::error_code, std::size_t){});
            timer_.expires_from_now(std::chrono::seconds{10});
            timer_.async_wait(this->strand_.wrap([this_](boost::system::error_code const& error) {
                if (!error) {
                    std::cout << "no response from switch";
                    this_->close(error);
                }
            }));
        }

    private:
        void handle_message(v13_detail::ofp_header const& header);

        void handle_disconnected(boost::system::error_code const& error)
        {
            if (this->endpoint_) {
                auto this_ = shared_from_this();
                this->thread_pool().post([this_, error]() {
                    v13_detail::get_base_type(this_->controller_handler_).handle(this_, disconnected_info{error});
                });
            }
        }

        void handle_hello(v13_detail::ofp_header const& header)
        {
            auto const hello_msg = decode<hello>(header);
            std::cout << boost::format{"handle_hello: %s"} % hello_msg;

            if (this->endpoint_) {
                std::cout << "received multiple-hello messages";
                return;
            }
            this->endpoint_ = socket().remote_endpoint();
            auto this_ = shared_from_this();
            this->thread_pool().post([this_]() {
                auto& controller_handler = this_->controller_handler_;
                v13_detail::get_base_type(controller_handler).handle(std::move(this_));
            });
        }

        void handle_error(v13_detail::ofp_header const& header)
        {
            auto error = decode<error_msg>(header);
            auto const it = this->reply_map_.find(error.xid());
            if (it != this->reply_map_.end() && error.failed_request_header().type == it->second->request_type()) {
                auto reply = it->second;
                this->thread_pool().post([=]() {
                    reply->error(to_error_code(error));
                });
                this->reply_map_.erase(it);
                return;
            }
            auto this_ = shared_from_this();
            this->thread_pool().post([this_, error]() mutable {
                auto& controller_handler = this_->controller_handler_;
                v13_detail::get_base_type(controller_handler).handle(std::move(this_), std::move(error));
            });
        }

        void handle_echo_request(v13_detail::ofp_header const& header)
        {
            auto echo_req = decode<echo_request>(header);
            std::cout << __func__ << ": " << echo_req;
            auto buffer_handler = detail::make_shared_buffer_handler([](boost::system::error_code, std::size_t){});
            this->async_send(boost::asio::buffer(std::move(echo_req).reply().encode(buffer_handler.buffer())), buffer_handler);
        }

        void handle_multipart();

        template <class T>
        void handle(v13_detail::ofp_header const& header)
        {
            auto message = decode<T>(header);
            auto this_ = shared_from_this();
            this->thread_pool().post([message, this_]() mutable {
                auto& controller_handler = this_->controller_handler_;
                v13_detail::get_base_type(controller_handler).handle(std::move(this_), std::move(message));
            });
        }

        template <class T>
        void handle_reply(v13_detail::ofp_header const& header)
        {
            auto message = decode<T>(header);
            auto const it = this->reply_map_.find(message.xid());
            if (it != this->reply_map_.end() && header.type == it->second->reply_type()) {
                auto txn = static_cast<transaction_impl<T>*>(it->second);
                this->thread_pool().post([=]() mutable {
                    txn->reply(std::move(message));
                });
                this->reply_map_.erase(it);
                return;
            }
            auto this_ = shared_from_this();
            this->thread_pool().post([this_, message]() mutable {
                auto& controller_handler = this_->controller_handler_;
                v13_detail::get_base_type(controller_handler).handle(std::move(this_), std::move(message));
            });
        }

    private:
        auto shared_from_this()
            -> std::shared_ptr<openflow_channel_impl>
        {
            return std::static_pointer_cast<openflow_channel_impl>(base_type::shared_from_this());
        }

        template <class T>
        auto decode(v13_detail::ofp_header const& header)
            -> T
        {
            auto first = boost::asio::buffer_cast<unsigned char const*>(streambuf_.data());
            auto const last = std::next(first, header.length);
            auto message = T::decode(first, last);
            if (first != last) {
                std::cout
                    << boost::format{"message length error (%s: length is %u but remained length is %u"}
                    % v13::to_string(protocol::ofp_type(header.type)) % header.length % std::distance(first, last);
            }
            streambuf_.consume(header.length);
            return message;
        }

    private:
        boost::asio::streambuf streambuf_;
        std::vector<unsigned char> send_buffer_;
        boost::asio::steady_timer timer_;
        ControllerHandler& controller_handler_;
    };

    template <class ControllerHandler, class Socket>
    inline void
    openflow_channel_impl<ControllerHandler, Socket>::handle_multipart()
    {
        auto multipart_reply = v13_detail::ofp_multipart_reply{};
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
        BOOST_PP_REPEAT(7, CANARD_NETWORK_OPENFLOW_V13_MULTIPART_TYPE_APPLY_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_MULTIPART_TYPE_APPLY_CASE

        default:
            [&]() {
                auto buf = std::vector<char>(multipart_reply.header.length);
                streambuf_.sgetn(&buf[0], buf.size());
                std::cout
                    << boost::format{"receive unknown multipart message (type=%s)"}
                    % to_string(protocol::ofp_multipart_type(multipart_reply.type));
            }();
        }
    }

    template <class ControllerHandler, class Socket>
    inline void
    openflow_channel_impl<ControllerHandler, Socket>::handle_message(v13_detail::ofp_header const& header)
    {
        if (!this->endpoint_ and header.type != protocol::OFPT_HELLO) {
            std::cout << "received non-hello message before hello";
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
        case protocol::OFPT_HELLO:
            handle_hello(header);
            break;
        case protocol::OFPT_ERROR:
            handle_error(header);
            break;
        case protocol::OFPT_ECHO_REQUEST:
            handle_echo_request(header);
            break;
        case protocol::OFPT_MULTIPART_REPLY:
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
        BOOST_PP_REPEAT(4, CANARD_NETWORK_OPENFLOW_V13_REPLY_MESSAGE_TYPE_APPLY_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_REPLY_MESSAGE_TYPE_APPLY_CASE

        default:
            [&]() {
                auto buf = std::vector<char>(header.length);
                streambuf_.sgetn(&buf[0], buf.size());
                std::cout
                    << boost::format{"receive unknown message (type=%u, length=%u)"}
                    % std::uint32_t(header.type) % header.length;
            }();
        }
    }

} // namespace v13

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CHANNLE_IMPL_HPP
