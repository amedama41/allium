#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/range/algorithm_ext/copy_n.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/system/error_code.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
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

    namespace secure_channel_detail {

        auto read_ofp_header(boost::asio::streambuf& streambuf)
            -> v10_detail::ofp_header
        {
            auto header = v10_detail::ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<std::uint8_t const*>(streambuf.data()), sizeof(header));
            return v10_detail::ntoh(header);
        }

        template <class Data, class Iterator>
        auto read(Iterator first, Iterator last)
            -> Data
        {
            auto data = Data{};
            boost::copy_n(
                      boost::make_iterator_range(first, last)
                    , sizeof(data)
                    , canard::as_byte_range(data).begin());
            return v10_detail::ntoh(data);
        }

    } // namespace secure_channel_detail

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

        void run(openflow::hello&& hello)
        {
            auto channel = base_type::shared_from_this();
            this->thread_pool().post([=]() mutable {
                handle(std::move(hello));
                auto loop = message_loop{
                    std::static_pointer_cast<secure_channel_impl>(channel)
                };
                loop.run();
            });
        }

    private:
        auto handle_read()
            -> std::size_t
        {
            while (streambuf_.size() >= sizeof(v10_detail::ofp_header)) {
                auto const header = secure_channel_detail::read_ofp_header(streambuf_);
                if (streambuf_.size() < header.length) {
                    return header.length - streambuf_.size();
                }
                handle_message(header);
                streambuf_.consume(header.length);
            }
            return sizeof(v10_detail::ofp_header) - streambuf_.size();
        }

        void handle_message(v10_detail::ofp_header const& header)
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
#           define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE(z, N, _) \
            using msg ## N = std::tuple_element<N, default_switch_message_list>::type; \
            case msg ## N::message_type: \
                handle(msg ## N::decode(first, last)); \
                break;
            static_assert(std::tuple_size<default_switch_message_list>::value == 7, "");
            BOOST_PP_REPEAT(7, CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE
            case OFPT_STATS_REPLY:
                handle_stats_reply(first, last);
                break;
            default:
                break;
            }
        }

        template <class Message>
        void handle(Message&& msg)
        {
            controller_handler_.handle(
                    base_type::shared_from_this(), std::forward<Message>(msg));
        }

        template <class Iterator>
        void handle_stats_reply(Iterator first, Iterator last)
        {
            auto const stats_reply = secure_channel_detail::read<v10_detail::ofp_stats_reply>(first, last);
            switch (stats_reply.type) {
#           define CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE(z, N, _) \
            using msg ## N = std::tuple_element<N, default_stats_reply_list>::type; \
            case msg ## N::stats_type_value: \
                handle(msg ## N::decode(first, last)); \
                break;
            static_assert(std::tuple_size<default_stats_reply_list>::value == 5, "");
            BOOST_PP_REPEAT(5, CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE
            default:
                break;
            }
        }

    private:
        struct message_loop
        {
            void run(std::size_t const least_size = sizeof(v10_detail::ofp_header))
            {
                auto const channel = channel_.get();
                channel->strand_.post(std::bind(std::move(*this), least_size));
            }

            void operator()()
            {
                auto const least_size = channel_->handle_read();
                run(least_size);
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
