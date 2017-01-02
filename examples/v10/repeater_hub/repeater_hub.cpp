#include <iostream>
#include <memory>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include "../match_creator.hpp"

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v10 = ofp::v10;

struct repeater_hub
{
  using versions = std::tuple<allium::v10::version>;

  template <class Channel>
  void handle(Channel const& channel, ofp::hello const& hello)
  {
    channel->async_send(v10::messages::set_config{
        v10::protocol::config_flags::normal, 0xffff
    });
  }

  template <class Channel>
  void handle(Channel const& channel, v10::messages::packet_in pkt_in)
  {
    channel->async_send(v10::messages::flow_add{
          match_from_packet(pkt_in.frame(), pkt_in.in_port()), 65535
        , 0x00000000
        , { v10::actions::output{v10::protocol::port_no::flood} }
        , ofp::v10::timeouts{0, 0}
        , v10::protocol::flow_mod_flags::send_flow_rem
        , v10::protocol::OFP_NO_BUFFER
    });

    channel->async_send(v10::messages::packet_out{
          pkt_in.extract_frame(), pkt_in.in_port()
        , { v10::actions::output{v10::protocol::port_no::table} }
    });
  }

  template <class Channel, class... Args>
  void handle(Channel const&, Args const&...) {}
};

int main()
{
  auto handler = repeater_hub{};
  using controller = allium::controller<repeater_hub>;
  try {
    controller cont{
        controller::options{handler}.address("0.0.0.0").port("6633")
    };
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

