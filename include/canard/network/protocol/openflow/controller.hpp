#ifndef CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP
#define CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
#include <canard/network/protocol/openflow/options.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel_impl.hpp>
#include <canard/network/protocol/openflow/with_buffer.hpp>
#include <canard/network/utils/io_service_pool.hpp>

#include <iostream>

namespace canard {
namespace network {
namespace openflow {

    template <class ControllerHandler>
    class controller
    {
        using tcp = boost::asio::ip::tcp;

    public:
        using options = controller_options<ControllerHandler>;
        using channel_ptr = std::shared_ptr<
            v10::secure_channel<
                detail::channel_data_t<ControllerHandler>, tcp::socket
            >
        >;

        controller(controller_options<ControllerHandler> const& options)
            : io_service_pool_(
                      options.io_service_pool()
                    ? options.io_service_pool()
                    : std::make_shared<utils::io_service_pool>(1))
            , io_service_{options.io_service()}
            , acceptor_{get_io_service()}
            , controller_handler_{options.handler()}
            , address_(options.address())
            , port_(options.port().empty() ? "6653" : options.port())
            , listening_mutex_{}
            , listening_{false}
        {
        }

        void run()
        {
            listen();
            auto work = utils::io_service_pool::work{*io_service_pool_};
            if (io_service_) {
                io_service_pool_->run(false);
                io_service_->run();
            }
            else {
                io_service_pool_->run(true);
            }
        }

        void stop()
        {
            std::lock_guard<std::mutex> lock{listening_mutex_};
            if (listening_) {
                if (io_service_) {
                    io_service_->stop();
                }
                io_service_pool_->stop();
            }
        }

        void listen()
        {
            std::lock_guard<std::mutex> lock{listening_mutex_};
            if (!listening_) {
                start_listening();
            }
            if (!listening_) {
                throw std::runtime_error{"Failed to listening"};
            }
        }

    private:
        auto get_io_service()
            -> boost::asio::io_service&
        {
            if (io_service_) {
                return *io_service_;
            }
            return io_service_pool_->get_io_service();
        }

        void async_accept()
        {
            auto socket = std::make_shared<tcp::socket>(
                    io_service_pool_->get_io_service());
            acceptor_.async_accept(
                    *socket, [=](boost::system::error_code const& ec) mutable {
                if (!ec) {
                    send_hello(std::move(socket));
                }
                else {
                    std::cout << "accept error: " << ec.message() << std::endl;
                }
                async_accept();
            });
        }

        void start_listening()
        {
            auto ec = boost::system::error_code{};
            tcp::resolver resolver{get_io_service()};
            auto const endpoint_iterator = resolver.resolve(
                    {address_, port_, tcp::resolver::query::passive}, ec);
            if (ec) {
                std::cout
                    << "resolve(" << address_ << ", " << port_ << ")"
                    << " error: " << ec.message() << std::endl;
                return;
            }
            auto const endpoint = (*endpoint_iterator).endpoint();
            if (acceptor_.open(endpoint.protocol(), ec)) {
                std::cout << "open error: " << ec.message() << std::endl;
                return;
            }
            if (acceptor_.bind(endpoint, ec)) {
                std::cout << "bind error: " << ec.message() << std::endl;
                return;
            }
            if (acceptor_.listen(tcp::acceptor::max_connections, ec)) {
                std::cout << "listen error: " << ec.message() << std::endl;
                return;
            }
            async_accept();
            listening_ = true;
        }

        void send_hello(std::shared_ptr<tcp::socket> socket)
        {
            auto buffer = std::make_shared<std::vector<std::uint8_t>>();
            auto const hello_msg = hello{v10::protocol::OFP_VERSION};
            boost::asio::async_write(
                      *socket
                    , with_buffer(hello_msg, *buffer).encode()
                    , [=](boost::system::error_code const& ec
                        , std::size_t const) mutable {
                if (ec) {
                    std::cout << "send error: " << ec.message() << std::endl;
                    return;
                }
                buffer->clear();
                receive_hello(std::move(socket), std::move(buffer));
            });
        }

        void receive_hello(std::shared_ptr<tcp::socket> socket
                , std::shared_ptr<std::vector<std::uint8_t>> buffer
                , std::size_t const read_size = sizeof(ofp_header))
        {
            auto const read_head = buffer->size();
            buffer->resize(read_head + read_size);
            boost::asio::async_read(
                      *socket
                    , boost::asio::buffer(&(*buffer)[read_head], read_size)
                    , boost::asio::transfer_exactly(read_size)
                    , [=](boost::system::error_code const& ec
                        , std::size_t const) {
                if (ec) {
                    std::cout << "read error: " << ec.message() << std::endl;
                    return;
                }
                auto const header
                    = detail::read_ofp_header(boost::asio::buffer(*buffer));
                if (header.type != hello::message_type) {
                    std::cout << "not hello message" << std::endl;
                    return;
                }
                if (header.length != buffer->size()) {
                    receive_hello(std::move(socket)
                                , std::move(buffer)
                                , header.length - buffer->size());
                    return;
                }
                auto strand
                    = boost::asio::io_service::strand{socket->get_io_service()};
                auto channel = std::make_shared<
                    v10::secure_channel_impl<ControllerHandler>
                >(std::move(*socket), strand, controller_handler_);
                auto it = buffer->begin();
                channel->run(hello::decode(it, buffer->end()));
            });
        }

    private:
        std::shared_ptr<utils::io_service_pool> io_service_pool_;
        std::shared_ptr<boost::asio::io_service> io_service_;
        tcp::acceptor acceptor_;
        ControllerHandler& controller_handler_;
        std::string address_;
        std::string port_;
        std::mutex listening_mutex_;
        bool listening_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP
