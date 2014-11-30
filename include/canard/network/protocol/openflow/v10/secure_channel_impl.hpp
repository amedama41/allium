#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/io/enum_to_string.hpp>
#include <canard/network/protocol/openflow/v10/messages.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include <canard/network/utils/thread_pool.hpp>

#include <iostream>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace detail {

        auto read_ofp_header(boost::asio::streambuf& streambuf)
            -> detail::ofp_header
        {
            auto header = detail::ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<std::uint8_t const*>(streambuf.data()), sizeof(header));
            return detail::ntoh(header);
        }

    } // namespace detail

    template <class ControllerHandler, class Socket = boost::asio::ip::tcp::socket>
    class secure_channel_impl
        : public secure_channel<Socket>
    {
        using base_type = secure_channel<Socket>;

    public:
        secure_channel_impl(
                  Socket socket
                , ControllerHandler& controller_handler
                , utils::thread_pool& thread_pool)
            : base_type{std::move(socket), thread_pool}
            , controller_handler_(controller_handler)
        {
        }

        ~secure_channel_impl()
        {
            std::cout << __func__ << std::endl;
        }

        void run()
        {
            auto loop = message_loop{
                std::static_pointer_cast<secure_channel_impl>(base_type::shared_from_this())
            };
            loop.run();
        }

    private:
        auto handle_read()
            -> std::size_t
        {
            while (streambuf_.size() >= sizeof(detail::ofp_header)) {
                auto const header = detail::read_ofp_header(streambuf_);
                if (streambuf_.size() < header.length) {
                    return header.length - streambuf_.size();
                }
                handle_message(header);
                streambuf_.consume(header.length);
            }
            return sizeof(detail::ofp_header) - streambuf_.size();
        }

        void handle_message(detail::ofp_header const& header)
        {
            std::cout
                << "version: " << std::uint16_t{header.version} << "\n"
                << "type:    " << v10::to_string(ofp_type(header.type)) << "\n"
                << "length:  " << header.length << "\n"
                << "xid:     " << header.xid << "\n"
                << std::endl;
            auto first = boost::asio::buffer_cast<unsigned char const*>(streambuf_.data());
            auto const last = std::next(first, header.length);
            switch (header.type) {
            case messages::error_msg::message_type:
                handle(messages::error_msg::decode(first, last));
                break;
            case messages::packet_in::message_type:
                handle(messages::packet_in::decode(first, last));
                break;
            // case messages::features_reply::message_type:
            //     handle(messages::features_reply::decode(first, last));
            //     break;
            default:
                break;
            }
        }

        void handle(messages::packet_in&& pkt_in)
        {
            std::cout
                << "    buffer_id:   " << pkt_in.buffer_id() << '\n'
                << "    in_port:     " << pkt_in.in_port() << '\n'
                << "    reason:      " << v10::to_string(pkt_in.reason()) << '\n'
                << std::endl;
            controller_handler_.handle(base_type::shared_from_this(), std::forward<messages::packet_in>(pkt_in));
        }

        template <class Message>
        void handle(Message&& msg)
        {
            std::cout
                << "    error_type:   " << v10::to_string(msg.error_type()) << "\n"
                << "    error_code:   " << msg.error_code() << "\n"
                << "    request type: " << v10::to_string(ofp_type(msg.failed_request_header().type)) << "\n"
                << std::endl;
        }

    private:
        struct message_loop
        {
            void run(std::size_t const least_size = sizeof(detail::ofp_header))
            {
                (*this)(least_size);
            }

            void operator()()
            {
                auto const least_size = channel_->handle_read();
                auto const channel = channel_.get();
                channel->strand_.dispatch(std::bind(std::move(*this), least_size));
            }

            void operator()(std::size_t const least_size)
            {
                auto const channel = channel_.get();
                boost::asio::async_read(channel->stream_, channel->streambuf_
                        , boost::asio::transfer_at_least(least_size)
                        , channel->strand_.wrap(std::move(*this)));
            }

            void operator()(boost::system::error_code const& ec, std::size_t)
            {
                if (ec) {
                    std::cout << "connection closed: " << ec.message() << " " << channel_.use_count() << std::endl;
                    return;
                }
                auto const channel = channel_.get();
                channel->thread_pool().post(std::move(*this));
            }

            std::shared_ptr<secure_channel_impl> channel_;
        };

    private:
        ControllerHandler& controller_handler_;
        boost::asio::streambuf streambuf_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
