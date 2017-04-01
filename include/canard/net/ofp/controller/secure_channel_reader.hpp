#ifndef CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_READER_HPP
#define CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_READER_HPP

#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/net/ofp/hello.hpp>
#include <canard/net/ofp/controller/decorator.hpp>
#include <canard/net/ofp/controller/goodbye.hpp>
#include <canard/net/ofp/controller/secure_channel.hpp>

#include <iostream>

namespace canard {
namespace net {
namespace ofp {
namespace controller {

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

    namespace detail {

        template <class ChannelDataMap, class Socket>
        class secure_channel_with_data
            : public secure_channel<Socket>
        {
        public:
            using secure_channel<Socket>::secure_channel;
            using channel_data_map = ChannelDataMap;

            template <class T>
            auto get_channel_data() noexcept
                -> detail::channel_data_t<T, channel_data_map>
            {
                return detail::get_channel_data<T>(data_);
            }

            template <class T>
            auto get_channel_data() const noexcept
                -> detail::channel_data_t<T, channel_data_map const>
            {
                return detail::get_channel_data<T>(data_);
            }

        private:
            channel_data_map data_;
        };

    } // namespace detail

    template <
          class MessageHandler
        , class ControllerHandler
        , class Socket
    >
    class secure_channel_reader
        : public detail::secure_channel_with_data<
              detail::channel_data_map_from_handler_t<ControllerHandler>, Socket
          >
    {
        using base_type = detail::secure_channel_with_data<
            detail::channel_data_map_from_handler_t<ControllerHandler>, Socket
        >;
        using channel_ptr = std::shared_ptr<secure_channel<Socket>>;
        using header_type = typename MessageHandler::header_type;

    public:
        secure_channel_reader(
                  Socket socket
                , boost::asio::io_service::strand strand
                , ControllerHandler& controller_handler)
            : base_type{std::move(socket), std::move(strand)}
            , controller_handler_(controller_handler)
        {
        }

        ~secure_channel_reader()
        {
            std::cout << __func__ << std::endl;
        }

        void run(net::ofp::hello&& hello)
        {
            auto base_channel = this->shared_from_this();
            handle(base_channel, std::move(hello));
            auto loop = message_loop{this, std::move(base_channel)};
            loop.run();
        }

    private:
        friend MessageHandler;

        template <class Message>
        void handle(channel_ptr const& channel, Message&& msg)
        {
            detail::handle(
                    controller_handler_, channel, std::forward<Message>(msg));
        }

    private:
        struct message_loop
        {
            void run()
            {
                auto const least_size = sizeof(header_type);
                reader_->strand_.dispatch(
                        canard::detail::bind(*this, least_size));
            }

            void operator()(std::size_t const least_size)
            {
                boost::asio::async_read(
                          reader_->stream_, reader_->streambuf_
                        , boost::asio::transfer_at_least(least_size)
                        , reader_->strand_.wrap(*this));
            }

            void operator()(boost::system::error_code const& ec, std::size_t)
            {
                if (ec) {
                    handle_read(reader_->streambuf_);
                    reader_->handle(base_channel_, goodbye{ec});
                    std::cout
                        << "connection closed: " << ec.message()
                        << " " << base_channel_.use_count() << std::endl;
                    return;
                }
                auto const least_size = handle_read(reader_->streambuf_);
                (*this)(least_size);
            }

            auto handle_read(boost::asio::streambuf& streambuf)
                -> std::size_t
            {
                while (streambuf.size() >= sizeof(header_type)) {
                    auto first = boost::asio::buffer_cast<
                        unsigned char const*
                    >(streambuf.data());

                    auto const header
                        = secure_channel_detail::read<header_type>(first);
                    if (streambuf.size() < header.length) {
                        return header.length - streambuf.size();
                    }

                    auto const last = std::next(first, header.length);
                    MessageHandler{}(
                            reader_, base_channel_, header, first, last);

                    streambuf.consume(header.length);
                }
                return sizeof(header_type) - streambuf.size();
            }

            friend auto asio_handler_allocate(
                    std::size_t const size, message_loop* const loop)
                -> void*
            {
                auto& storage = loop->reader_->storage_;
                if (!storage.used && size <= sizeof(storage.memory)) {
                    storage.used = true;
                    return std::addressof(storage.memory);
                }
                return operator new(size);
            }

            friend void asio_handler_deallocate(
                    void* const pointer, std::size_t, message_loop* const loop)
            {
                auto& storage = loop->reader_->storage_;
                if (pointer == std::addressof(storage.memory)) {
                    storage.used = false;
                    return;
                }
                operator delete(pointer);
            }

            secure_channel_reader* reader_;
            channel_ptr base_channel_;
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

} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_READER_HPP
