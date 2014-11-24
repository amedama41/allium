#ifndef CANARD_NETWORK_OPENFLOW_OPTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_OPTIONS_HPP

#include <string>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <canard/network/utils/thread_pool.hpp>

namespace canard {
namespace network {
namespace openflow {

    template <class ControllerHandler>
    class controller_options
    {
    public:
        explicit controller_options(ControllerHandler& handler)
            : io_service_{}
            , handler_(handler)
        {
        }

        auto handler() const
            -> ControllerHandler&
        {
            return handler_;
        }

        auto io_service() const
            -> std::shared_ptr<boost::asio::io_service>
        {
            return io_service_;
        }

        auto io_service(std::shared_ptr<boost::asio::io_service> io_service_ptr)
            -> controller_options&
        {
            io_service_ = std::move(io_service_ptr);
            return *this;
        }

        auto thread_pool() const
            -> std::shared_ptr<utils::thread_pool>
        {
            return thread_pool_;
        }

        auto thread_pool(std::shared_ptr<utils::thread_pool> thread_pool_ptr)
            -> controller_options&
        {
            thread_pool_ = std::move(thread_pool_ptr);
            return *this;
        }

        auto address() const
            -> std::string
        {
            return address_;
        }

        auto address(std::string const& address)
            -> controller_options&
        {
            address_ = address;
            return *this;
        }

        auto port() const
            -> std::string
        {
            return port_;
        }

        auto port(std::string const& port)
            -> controller_options&
        {
            port_ = port;
            return *this;
        }

    private:
        std::shared_ptr<boost::asio::io_service> io_service_;
        ControllerHandler& handler_;
        std::string address_;
        std::string port_;
        std::shared_ptr<utils::thread_pool> thread_pool_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_OPTIONS_HPP
