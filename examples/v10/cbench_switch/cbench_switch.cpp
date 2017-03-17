#include <iostream>
#include <string>
#include <tuple>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include <canard/network/utils/io_service_pool.hpp>
#include "../match_creator.hpp"

namespace allium = canard::net::ofp::controller;
namespace ofp = canard::net::ofp;
namespace v10 = ofp::v10;

struct cbench_switch
{
  using versions = std::tuple<allium::v10::version>;

  template <class Channel>
  void handle(Channel const& channel, ofp::hello const&)
  {
    channel->async_send(v10::messages::set_config{
        v10::protocol::config_flags::normal, 0xffff
    });
    channel->async_send(v10::messages::features_request{});
  }

  template <class Channel>
  void handle(Channel const& channel
            , v10::messages::packet_in const& pkt_in)
  {
      channel->async_send(v10::messages::flow_add{
            match_from_packet(pkt_in.frame(), pkt_in.in_port())
          , 65535
          , 0
          , { v10::actions::output(pkt_in.in_port() + 1) }
          , { v10::protocol::OFP_FLOW_PERMANENT, v10::protocol::OFP_FLOW_PERMANENT }
          , 0, pkt_in.buffer_id()
      });
  }

  template <class... Args>
  void handle(Args const&...) {}
};

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cout
      << "Usage: " << argv[0]
      << " #io_services #threads_per_iosrv" << std::endl;
    return -1;
  }
  auto const num_io_service = std::stoul(argv[1]);
  auto const num_thread_per_iosrv = std::stoul(argv[2]);

  cbench_switch handler{};

  using canard::net::utils::io_service_pool;
  auto pool = std::make_shared<io_service_pool>(
      num_io_service, num_thread_per_iosrv);

  using controller = allium::controller<cbench_switch>;
  controller cont{
    controller::options{handler}
      .address("0.0.0.0").port("6653").io_service_pool(pool)
  };
  try {
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    cont.stop();
  }
}

