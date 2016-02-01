#ifndef CANARD_NETWORK_OPENFLOW_SETUP_CONNECTION_HPP
#define CANARD_NETWORK_OPENFLOW_SETUP_CONNECTION_HPP

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/system/error_code.hpp>
#include <boost/utility/string_ref.hpp>
#include <canard/asio/asio_handler_hook_propagation.hpp>
#include <canard/network/protocol/openflow/error.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
#include <canard/network/protocol/openflow/with_buffer.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    namespace setup_connection_detail {

        auto is_valid_hello(openflow::ofp_header const header)
            -> bool
        {
            if (header.type != openflow::hello::message_type) {
                std::cout << "not hello message: " << std::uint32_t{header.type} << std::endl;
                return false;
            }

            if (header.length < sizeof(openflow::ofp_header)) {
                std::cout << "invalid message length: " << header.length << std::endl;
                return false;
            }

            return true;
        }

        class timer
        {
            using timer_type = boost::asio::steady_timer;

        public:
            using duration = timer_type::duration;

            explicit timer(boost::asio::io_service& io_service)
                : timer_{io_service}
                , timeout_id_{0}
            {
            }

            void expires_from_now(duration const& expiry_time)
            {
                timer_.expires_from_now(expiry_time);
                ++timeout_id_;
            }

            template <class Handler>
            void async_wait(Handler h)
            {
                timer_.async_wait(
                        timeout_check_handler<Handler>{this, std::move(h)});
            }

            void cancel()
            {
                ++timeout_id_;
                timer_.cancel();
            }

            template <class Handler>
            struct timeout_check_handler
                : asio_handler_hook_propagation<timeout_check_handler<Handler>>
            {
                timeout_check_handler(timer* const this_, Handler&& handler)
                    : this_(this_)
                    , timeout_id_(this_->timeout_id_)
                    , handler_(std::move(handler))
                {
                }

                auto handler() noexcept
                    -> Handler&
                {
                    return handler_;
                }

                void operator()(boost::system::error_code ec)
                {
                    if (!ec && this_->timeout_id_ != timeout_id_) {
                        ec = boost::asio::error::operation_aborted;
                    }
                    handler_(ec);
                }

                timer* this_;
                std::size_t timeout_id_;
                Handler handler_;
            };

        private:
            timer_type timer_;
            std::size_t timeout_id_;
        };

        struct version_bitmap_creator
        {
            version_bitmap_creator()
                : bitmap{}
            {
            }

            template <class Version>
            void operator()(Version)
            {
                if (bitmap.size() <= Version::value) {
                    bitmap.resize(Version::value + 1);
                }
                bitmap.set(Version::value);
            }

            boost::dynamic_bitset<std::uint32_t> bitmap;
        };

        template <class SupportedVersions>
        auto make_version_bitmap()
            -> std::vector<std::uint32_t>
        {
            auto creator = version_bitmap_creator{};
            boost::fusion::for_each(SupportedVersions{}, std::ref(creator));
            auto bitmap
                = std::vector<std::uint32_t>(creator.bitmap.num_blocks());
            boost::to_block_range(creator.bitmap, bitmap.begin());
            return bitmap;
        }

    } // namespace setup_connection_detail

    template <class ControllerHandler>
    class setup_connection
        : public std::enable_shared_from_this<
            setup_connection<ControllerHandler>
          >
    {
        using supported_versions = typename ControllerHandler::versions;
        using tcp = boost::asio::ip::tcp;

        enum { timeout = 30 };
    public:
        setup_connection(
                ControllerHandler& handler
              , boost::asio::io_service& io_service)
            : handler_(handler)
            , socket_{io_service}
            , timer_{io_service}
            , strand_{io_service}
            , buffer_{}
            , endpoint_{}
        {
        }

        auto socket() noexcept
            -> tcp::socket&
        {
            return socket_;
        }

        auto endpoint() noexcept
            -> tcp::endpoint&
        {
            return endpoint_;
        }

        void start_setup()
        {
            auto self = this->shared_from_this();
            strand_.post([this, self]{
                async_send_hello(self);
                set_connection_timeout(self);
            });
        }

    private:
        void close(boost::string_ref const& reason)
        {
            auto ignore = boost::system::error_code{};
            socket_.close(ignore);
            std::cout << reason << std::endl;
        }

        void async_send_hello(std::shared_ptr<setup_connection> const& self)
        {
            auto hello = openflow::hello{
                setup_connection_detail::make_version_bitmap<supported_versions>()
            };
            boost::asio::async_write(
                    socket_
                  , openflow::with_buffer(std::move(hello), buffer_).encode()
                  , strand_.wrap([this, self](
                          boost::system::error_code const& ec, std::size_t) {
                cancel_connection_timeout();
                if (ec) {
                    close("failed to send hello: " + ec.message());
                    return;
                }
                async_receive_hello(self);
                set_connection_timeout(self);
            }));
        }

        void async_receive_hello(std::shared_ptr<setup_connection> const& self)
        {
            boost::asio::async_read(
                    socket_, boost::asio::buffer(buffer_)
                  , boost::asio::transfer_exactly(sizeof(openflow::ofp_header))
                  , strand_.wrap([this, self](
                          boost::system::error_code const& ec, std::size_t) {
                cancel_connection_timeout();
                if (ec) {
                    close("failed to receive hello: " + ec.message());
                    return;
                }

                auto const header
                    = detail::read_ofp_header(boost::asio::buffer(buffer_));
                if (!setup_connection_detail::is_valid_hello(header)) {
                    close("received invalid hello message");
                    return;
                }

                auto const header_length = sizeof(openflow::ofp_header);
                if (header.length == header_length) {
                    buffer_.resize(header_length);
                    handle_hello(self);
                }
                else {
                    async_receive_hello_elements(
                            self, header.length - header_length);
                    set_connection_timeout(self);
                }
            }));
        }

        void async_receive_hello_elements(
                std::shared_ptr<setup_connection> const& self
              , std::size_t const elements_length)
        {
            auto const header_length = sizeof(openflow::ofp_header);

            buffer_.resize(header_length + elements_length);
            boost::asio::async_read(
                    socket_
                  , boost::asio::buffer(
                        buffer_.data() + header_length, elements_length)
                  , boost::asio::transfer_exactly(elements_length)
                  , strand_.wrap([this, self](
                          boost::system::error_code const& ec, std::size_t) {
                cancel_connection_timeout();
                if (ec) {
                    close("failed to receive hello elements: " + ec.message());
                    return;
                }
                handle_hello(self);
            }));
        }

        struct channel_starter
        {
            template <class Version>
            void operator()(Version)
            {
                if (!has_supported_version && hello.support(Version::value)) {
                    has_supported_version = true;
                    using channel_type = typename Version::template channel_t<
                        ControllerHandler, tcp::socket
                    >;
                    auto const channel = std::make_shared<channel_type>(
                            std::move(connection.socket_)
                          , connection.strand_, connection.handler_);
                    channel->run(std::move(hello));
                }
            }

            setup_connection& connection;
            openflow::hello& hello;
            bool has_supported_version;
        };

        void handle_hello(std::shared_ptr<setup_connection> const& self)
        {
            auto it = buffer_.begin();
            auto hello = openflow::hello::decode(it, buffer_.end());

            auto starter = channel_starter{*this, hello, false};
            boost::fusion::for_each(supported_versions{}, std::ref(starter));
            if (!starter.has_supported_version) {
                async_send_incompatible_error(self, hello.xid());
            }
        }

        void async_send_incompatible_error(
                std::shared_ptr<setup_connection> const& self
              , std::uint32_t const xid)
        {
            auto error = openflow::error{
                openflow::OFPET_HELLO_FAILED, openflow::OFPHFC_INCOMPATIBLE, {}//, xid
            };
            boost::asio::async_write(
                    socket_
                  , openflow::with_buffer(std::move(error), buffer_).encode()
                  , strand_.wrap([this, self](
                          boost::system::error_code const& ec, std::size_t) {
                close("incompatible openflow version");
            }));
        }

        void set_connection_timeout(
                std::shared_ptr<setup_connection> const& self)
        {
            timer_.expires_from_now(std::chrono::seconds{timeout});
            timer_.async_wait(
                    strand_.wrap([this, self](
                            boost::system::error_code const& ec) {
                if (!ec) {
                    close("connection timeout");
                }
            }));
        }

        void cancel_connection_timeout()
        {
            timer_.cancel();
        }

    private:
        ControllerHandler& handler_;
        tcp::socket socket_;
        setup_connection_detail::timer timer_;
        boost::asio::io_service::strand strand_;
        std::vector<unsigned char> buffer_;
        tcp::endpoint endpoint_;
    };

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_SETUP_CONNECTION_HPP
