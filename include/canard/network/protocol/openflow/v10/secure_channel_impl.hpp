#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_IMPL_HPP

#include <cstddef>
#include <cstring>
#include <iterator>
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
#include <boost/system/error_code.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/goodbye.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/messages.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>

#include <iostream>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace secure_channel_detail {

        template <class Data>
        auto read(unsigned char const* const src)
            -> Data
        {
            auto data = Data{};
            std::memcpy(&data, src, sizeof(data));
            boost::endian::big_to_native_inplace(data);
            return data;
        }

    } // namespace secure_channel_detail

    template <
          class ControllerHandler
        , class ChannelData = detail::channel_data_t<ControllerHandler>
        , class Socket = boost::asio::ip::tcp::socket
    >
    class secure_channel_impl
        : public secure_channel<ChannelData, Socket>
    {
        using base_type = secure_channel<ChannelData, Socket>;

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
            auto const read_channel
                = static_cast<secure_channel_impl*>(base_channel.get());
            auto loop = message_loop{read_channel, std::move(base_channel)};
            loop.run();
        }

    private:
        template <class Message>
        void handle(std::shared_ptr<base_type> const& channel, Message&& msg)
        {
            detail::handle(
                    controller_handler_, channel, std::forward<Message>(msg));
        }

    private:
        struct message_loop
        {
            void run()
            {
                auto const least_size = sizeof(v10_detail::ofp_header);
                channel_->strand_.dispatch(
                        canard::detail::bind(std::move(*this), least_size));
            }

            void operator()(std::size_t const least_size)
            {
                boost::asio::async_read(
                          channel_->stream_, channel_->streambuf_
                        , boost::asio::transfer_at_least(least_size)
                        , channel_->strand_.wrap(std::move(*this)));
            }

            void operator()(boost::system::error_code const& ec, std::size_t)
            {
                if (ec) {
                    handle_read(channel_->streambuf_);
                    channel_->handle(base_channel_, openflow::goodbye{ec});
                    channel_->close();
                    std::cout
                        << "connection closed: " << ec.message()
                        << " " << base_channel_.use_count() << std::endl;
                    return;
                }
                auto const least_size = handle_read(channel_->streambuf_);
                (*this)(least_size);
            }

            auto handle_read(boost::asio::streambuf& streambuf)
            -> std::size_t
            {
                while (streambuf.size() >= sizeof(v10_detail::ofp_header)) {
                    auto first = boost::asio::buffer_cast<
                        unsigned char const*
                    >(streambuf.data());

                    auto const header = secure_channel_detail::read<
                        v10_detail::ofp_header
                    >(first);
                    if (streambuf.size() < header.length) {
                        return header.length - streambuf.size();
                    }

                    auto const last = std::next(first, header.length);
                    handle_message(header, first, last);

                    streambuf.consume(header.length);
                }
                return sizeof(v10_detail::ofp_header) - streambuf.size();
            }

            void handle_message(
                      v10_detail::ofp_header const& header
                    , unsigned char const* first
                    , unsigned char const* const last)
            {
                switch (header.type) {
#               define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE(z, N, _) \
                using msg ## N \
                    = std::tuple_element<N, default_switch_message_list>::type; \
                case msg ## N::message_type: \
                    channel_->handle( \
                            base_channel_, msg ## N::decode(first, last)); \
                    break;
                static_assert(
                          std::tuple_size<default_switch_message_list>::value
                          == 10
                        , "not match to the number of message types");
                BOOST_PP_REPEAT(
                        10, CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE, _)
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

            void handle_stats_reply(
                      unsigned char const* first
                    , unsigned char const* const last)
            {
                auto const stats_reply = secure_channel_detail::read<
                    v10_detail::ofp_stats_reply
                >(first);
                switch (stats_reply.type) {
#               define CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE(z, N, _) \
                using msg ## N \
                    = std::tuple_element<N, default_stats_reply_list>::type; \
                case msg ## N::stats_type_value: \
                    channel_->handle( \
                            base_channel_, msg ## N::decode(first, last)); \
                    break;
                static_assert(
                          std::tuple_size<default_stats_reply_list>::value == 6
                        , "not match to the number of stats reply types");
                BOOST_PP_REPEAT(
                        6, CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE, _)
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

            secure_channel_impl* channel_;
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
