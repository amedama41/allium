#include <iostream>
#include <tuple>
#include <utility>
#include <boost/format.hpp>
#include <canard/network/openflow/v10/io/openflow.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v10 = ofp::v10;

template <class Logger, class Base>
struct logging : public Base
{
  explicit logging(Logger logger)
    : logger(logger)
  {
  }

  template <class Channel, class Message>
  void handle(Channel&& c, Message&& m)
  {
    logger
      << boost::format("received %||") % v10::protocol::msg_type(m.type())
      << std::endl;
    this->forward(std::forward<Channel>(c), std::forward<Message>(m));
  }

  template <class Channel>
  void handle(Channel&& c, allium::goodbye m)
  {
    logger
      << boost::format("disconnected because of %||") % m.reason().message()
      << std::endl;
    this->forward(std::forward<Channel>(c), std::move(m));
  }

  Logger logger;
};

template <class Base>
struct auto_negotiate : public Base
{
  template <class Channel>
  void handle(Channel&& c, ofp::hello&& hello)
  {
    c->async_send(v10::messages::features_request{});
    this->forward(std::forward<Channel>(c), std::move(hello));
  }

  template <class Channel>
  void handle(Channel&& c, v10::messages::features_reply&& features)
  {
    using boost::format;
    std::cout << format("  dpid: 0x%|016x|\n") % features.datapath_id();
    std::cout << format("  n_buffers: %||\n") % features.num_buffers();
    std::cout << format("  capabilities: %|08x|\n") % features.capabilities();
    std::cout << format("  actions: %|08x|\n") % features.actions();
    std::cout << format("  num_ports: %||") % features.ports().size();
    std::cout << std::endl;
    c->async_send(
        v10::messages::set_config{v10::protocol::config_flags::normal, 0xffff});
    this->forward(std::forward<Channel>(c), std::move(features));
  }

  template <class... Args>
  void handle(Args&&... args)
  {
    this->forward(std::forward<Args>(args)...);
  }
};

template <class Base>
struct auto_echo : public Base
{
  template <class Channel>
  void handle(Channel&& c, v10::messages::echo_request&& echo)
  {
    std::cout << "reply echo" << std::endl;
    c->async_send(v10::messages::echo_reply{std::move(echo)});
  }

  template <class... Args>
  void handle(Args&&... args)
  {
    this->forward(std::forward<Args>(args)...);
  }
};

template <class Base>
using ostream_logging = logging<std::ostream&, Base>;

struct decorator_switch
  : public allium::decorate<
      decorator_switch, auto_negotiate, auto_echo, ostream_logging
    >
{
  using versions = std::tuple<allium::v10::version>;

  decorator_switch()
    : decorate{allium::make_args<ostream_logging>(std::cout)}
  {
  }

  template <class Channel>
  void handle(Channel const& channel, v10::messages::packet_in&& pkt_in)
  {
    channel->async_send(v10::messages::packet_out{
          pkt_in.extract_frame(), pkt_in.in_port()
        , { v10::actions::output{v10::protocol::port_no::flood} }
    });
  }

  template <class Channel, class... Args>
  void handle(Channel const& channel, Args&&...)
  {
  }
};

int main(int argc, char* argv[])
{
  auto handler = decorator_switch{};
  using controller = allium::controller<decorator_switch>;
  controller cont{
    controller::options{handler}
      .address("0.0.0.0").port(argc < 2 ? "6653" : argv[1])
  };
  try {
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

