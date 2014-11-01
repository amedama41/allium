#ifndef CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel_impl.hpp>
#include <canard/network/protocol/openflow/v13/options.hpp>
#include <canard/network/utils/thread_pool.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    template <class ControllerHandler>
    class controller
    {
        using tcp = boost::asio::ip::tcp;

    public:
        using channel_ptr = std::shared_ptr<openflow_channel<>>;
        using options = controller_options<ControllerHandler>;

        controller(controller_options<ControllerHandler> const& options)
            : io_service_{options.io_service() ? options.io_service() : std::make_shared<boost::asio::io_service>()}
            , acceptor_{*io_service_}
            , controller_handler_{options.handler()}
            , thread_pool_(options.thread_pool() ? options.thread_pool() : std::make_shared<utils::thread_pool>(1))
            , address_(options.address())
            , port_(options.port().empty() ? "6653" : options.port())
            , listening_mutex_{}
            , listening_{false}
        {
        }

        void run()
        {
            listen();
            io_service_->run();
        }

        void stop()
        {
            std::lock_guard<std::mutex> lock{listening_mutex_};
            if (listening_) {
                io_service_->stop();
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
            auto channel = std::make_shared<openflow_channel_impl<ControllerHandler>>(
                    *io_service_, controller_handler_, *thread_pool_);
            acceptor_.async_accept(channel->socket(), [=](boost::system::error_code const& error) {
                if (!error) {
                    channel->run();
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

    private:
        std::shared_ptr<boost::asio::io_service> io_service_;
        tcp::acceptor acceptor_;
        ControllerHandler& controller_handler_;
        std::shared_ptr<utils::thread_pool> thread_pool_;
        std::string address_;
        std::string port_;
        std::mutex listening_mutex_;
        bool listening_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_HPP
