#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/range/algorithm_ext/copy_n.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/system/error_code.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/network/protocol/openflow/goodbye.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/io/enum_to_string.hpp>
#include <canard/network/protocol/openflow/v10/messages.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>

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
            std::memcpy(&header
                      , boost::asio::buffer_cast<std::uint8_t const*>(
                          streambuf.data()), sizeof(header));
            boost::endian::big_to_native_inplace(header);
            return header;
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
            boost::endian::big_to_native_inplace(data);
            return data;
        }

    } // namespace secure_channel_detail

    template <
          class ControllerHandler
        , class Socket = boost::asio::ip::tcp::socket
    >
    class secure_channel_impl
        : public secure_channel<Socket>
    {
        using base_type = secure_channel<Socket>;

    public:
        secure_channel_impl(
                  Socket socket
                , boost::asio::io_service::strand strand
                , ControllerHandler& controller_handler)
            : base_type{std::move(socket), std::move(strand)}
            , controller_handler_(controller_handler)
        {
        }

        ~secure_channel_impl()
        {
            std::cout << __func__ << std::endl;
        }

        void run(openflow::hello&& hello)
        {
            auto base_channel = base_type::shared_from_this();
            handle(base_channel, std::move(hello));
            auto read_channel
                = std::static_pointer_cast<secure_channel_impl>(base_channel);
            auto loop = message_loop{
                std::move(read_channel), std::move(base_channel)
            };
            loop.run();
        }

    private:
        template <class Message>
        void handle(std::shared_ptr<base_type> const& channel, Message&& msg)
        {
            controller_handler_.handle(channel, std::forward<Message>(msg));
        }

    private:
        struct message_loop
        {
            void run()
            {
                auto const channel = channel_.get();
                auto const least_size = sizeof(v10_detail::ofp_header);
                channel->strand_.dispatch(
                        canard::detail::bind(std::move(*this), least_size));
            }

            void operator()(std::size_t const least_size)
            {
                auto const channel = channel_.get();
                boost::asio::async_read(
                          channel->stream_, channel->streambuf_
                        , boost::asio::transfer_at_least(least_size)
                        , channel->strand_.wrap(std::move(*this)));
            }

            void operator()(boost::system::error_code const& ec, std::size_t)
            {
                if (ec) {
                    handle_read(channel_->streambuf_);
                    channel_->handle(base_channel_, openflow::goodbye{ec});
                    channel_->close();
                    std::cout
                        << "connection closed: " << ec.message()
                        << " " << channel_.use_count() << std::endl;
                    return;
                }
                auto const least_size = handle_read(channel_->streambuf_);
                (*this)(least_size);
            }

            auto handle_read(boost::asio::streambuf& streambuf)
            -> std::size_t
            {
                while (streambuf.size() >= sizeof(v10_detail::ofp_header)) {
                    auto const header
                        = secure_channel_detail::read_ofp_header(streambuf);
                    if (streambuf.size() < header.length) {
                        return header.length - streambuf.size();
                    }

                    auto first = boost::asio::buffer_cast<
                        unsigned char const*
                    >(streambuf.data());
                    auto const last = std::next(first, header.length);
                    handle_message(header, first, last);

                    streambuf.consume(header.length);
                }
                return sizeof(v10_detail::ofp_header) - streambuf.size();
            }

            template <class Iterator>
            void handle_message(
                      v10_detail::ofp_header const& header
                    , Iterator first, Iterator last)
            {
                // std::cout
                //     << "version: " << std::uint16_t{header.version} << "\n"
                //     << "type:    " << v10::to_string(protocol::ofp_type(header.type)) << "\n"
                //     << "length:  " << header.length << "\n"
                //     << "xid:     " << header.xid << "\n"
                //     << std::endl;
                switch (header.type) {
#               define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE(z, N, _) \
                using msg ## N = std::tuple_element<N, default_switch_message_list>::type; \
                case msg ## N::message_type: \
                    channel_->handle(base_channel_, msg ## N::decode(first, last)); \
                    break;
                static_assert(
                          std::tuple_size<default_switch_message_list>::value == 10
                        , "");
                BOOST_PP_REPEAT(10, CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE, _)
#               undef  CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE
                case protocol::OFPT_STATS_REPLY:
                    if (header.length < sizeof(v10_detail::ofp_stats_reply)) {
                        // TODO needs error handling
                        break;
                    }
                    handle_stats_reply(first, last);
                    break;
                default:
                    break;
                }
            }

            template <class Iterator>
            void handle_stats_reply(Iterator first, Iterator last)
            {
                auto const stats_reply = secure_channel_detail::read<
                    v10_detail::ofp_stats_reply
                >(first, last);
                switch (stats_reply.type) {
#               define CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE(z, N, _) \
                using msg ## N = std::tuple_element<N, default_stats_reply_list>::type; \
                case msg ## N::stats_type_value: \
                    channel_->handle(base_channel_, msg ## N::decode(first, last)); \
                    break;
                static_assert(
                          std::tuple_size<default_stats_reply_list>::value == 6
                        , "");
                BOOST_PP_REPEAT(6, CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE, _)
#               undef  CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE
                default:
                    break;
                }
            }

            friend auto asio_handler_allocate(
                    std::size_t const size, message_loop* const loop)
                -> void*
            {
                auto& storage = loop->channel_->storage_;
                if (!storage.used && size <= sizeof(storage.memory)) {
                    storage.used = true;
                    return std::addressof(storage.memory);
                }
                return operator new(size);
            }

            friend void asio_handler_deallocate(
                    void* const pointer, std::size_t, message_loop* const loop)
            {
                auto& storage = loop->channel_->storage_;
                if (pointer == std::addressof(storage.memory)) {
                    storage.used = false;
                    return;
                }
                operator delete(pointer);
            }

            std::shared_ptr<secure_channel_impl> channel_;
            std::shared_ptr<base_type> base_channel_;
        };

    private:
        ControllerHandler& controller_handler_;
        boost::asio::streambuf streambuf_;
        struct read_handler_storage
        {
            typename std::aligned_storage<256>::type memory;
            bool used = false;
        } storage_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
