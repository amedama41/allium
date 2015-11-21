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
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/hello.hpp>
#include <canard/network/protocol/openflow/options.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel_impl.hpp>
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
        using channel_ptr = std::shared_ptr<v10::secure_channel<>>;
        using options = controller_options<ControllerHandler>;

        controller(controller_options<ControllerHandler> const& options)
            : io_service_{options.io_service() ? options.io_service() : std::make_shared<boost::asio::io_service>()}
            , acceptor_{*io_service_}
            , controller_handler_{options.handler()}
            , io_service_pool_(
                      options.io_service_pool()
                    ? options.io_service_pool()
                    : std::make_shared<utils::io_service_pool>(1))
            , address_(options.address())
            , port_(options.port().empty() ? "6653" : options.port())
            , listening_mutex_{}
            , listening_{false}
        {
        }

        void run()
        {
            listen();
            io_service_pool_->run();
            io_service_->run();
        }

        void stop()
        {
            std::lock_guard<std::mutex> lock{listening_mutex_};
            if (listening_) {
                io_service_->stop();
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
        void async_accept()
        {
            auto socket = std::make_shared<tcp::socket>(
                    io_service_pool_->get_io_service());
            acceptor_.async_accept(*socket, [=](boost::system::error_code const& error) mutable {
                if (!error) {
                    send_hello(std::move(socket));
                }
                else {
                    std::cout << "accept error: " << error.message() << std::endl;
                }
                async_accept();
            });
        }

        void start_listening()
        {
            auto ec = boost::system::error_code{};
            tcp::resolver resolver{*io_service_};
            auto const endpoint_iterator = resolver.resolve({address_, port_}, ec);
            if (ec) {
                std::cout << "resolve(" << address_ << ", " << port_ << ") error: " << ec.message() << std::endl;
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
            boost::asio::async_write(*socket, boost::asio::buffer(hello{v10::protocol::OFP_VERSION}.encode(*buffer))
                    , [=](boost::system::error_code const& ec, std::size_t const) mutable {
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
            boost::asio::async_read(*socket
                    , boost::asio::buffer(&(*buffer)[read_head], read_size)
                    , boost::asio::transfer_exactly(read_size)
                    , [=](boost::system::error_code const& ec, std::size_t const) {
                if (ec) {
                    std::cout << "read error: " << ec.message() << std::endl;
                    return;
                }
                auto const header = detail::read_ofp_header(boost::asio::buffer(*buffer));
                if (header.type != hello::message_type) {
                    std::cout << "not hello message" << std::endl;
                    return;
                }
                if (header.length != buffer->size()) {
                    receive_hello(std::move(socket), std::move(buffer), header.length - buffer->size());
                    return;
                }
                auto channel = std::make_shared<
                    v10::secure_channel_impl<ControllerHandler>
                >(std::move(*socket), controller_handler_);
                auto it = buffer->begin();
                channel->run(hello::decode(it, buffer->end()));
            });
        }

    private:
        std::shared_ptr<boost::asio::io_service> io_service_;
        tcp::acceptor acceptor_;
        ControllerHandler& controller_handler_;
        std::shared_ptr<utils::io_service_pool> io_service_pool_;
        std::string address_;
        std::string port_;
        std::mutex listening_mutex_;
        bool listening_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP
