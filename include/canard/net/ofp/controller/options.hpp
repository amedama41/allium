#ifndef CANARD_NETWORK_OPENFLOW_OPTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_OPTIONS_HPP

#include <string>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <canard/net/utils/io_service_pool.hpp>

namespace canard {
namespace net {
namespace ofp {
namespace controller {

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

    auto io_service_pool() const
      -> std::shared_ptr<utils::io_service_pool>
    {
      return io_service_pool_;
    }

    auto io_service_pool(
        std::shared_ptr<utils::io_service_pool> io_service_pool)
      -> controller_options&
    {
      io_service_pool_ = std::move(io_service_pool);
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
    std::shared_ptr<utils::io_service_pool> io_service_pool_;
  };

} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_OPTIONS_HPP
