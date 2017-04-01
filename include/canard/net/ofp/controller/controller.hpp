#ifndef CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP
#define CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <canard/net/ofp/controller/decorator.hpp>
#include <canard/net/ofp/controller/options.hpp>
#include <canard/net/ofp/controller/setup_connection.hpp>
#include <canard/net/utils/io_service_pool.hpp>

#include <iostream>

namespace canard {
namespace net {
namespace ofp {
namespace controller {

    template <class ControllerHandler>
    class controller
    {
        using tcp = boost::asio::ip::tcp;

    public:
        using options = controller_options<ControllerHandler>;

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
            using setup_connection
                = detail::setup_connection<ControllerHandler>;
            auto connection = std::make_shared<setup_connection>(
                    controller_handler_, io_service_pool_->get_io_service());
            acceptor_.async_accept(
                    connection->socket(), connection->endpoint()
                  , [=](boost::system::error_code const& ec) mutable {
                if (!ec) {
                    connection->start_setup();
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

} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_CONTROLLER_HPP
