#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include <canard/network/utils/io_service_pool.hpp>
#include "../match_creator.hpp"

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v10 = ofp::v10;

template <class Base>
struct auto_negotiate : public Base
{
  template <class Channel>
  void handle(Channel&& channel, ofp::hello&& hello)
  {
    channel->async_send(v10::messages::features_request{});
    channel->async_send(v10::messages::set_config{
        0xffff, v10::protocol::OFPC_FRAG_NORMAL
    });
    this->forward(std::forward<Channel>(channel), std::move(hello));
  }

  template <class... Args>
  void handle(Args&&... args)
  {
    this->forward(std::forward<Args>(args)...);
  }
};

struct learning_switch
  : public allium::decorate<learning_switch, auto_negotiate>
{
  using versions = std::tuple<allium::v10::version>;
  using channel_data = std::map<canard::mac_address, std::uint32_t>;

  template <class Channel>
  void handle(Channel const& channel, v10::messages::packet_in&& pkt_in)
  {
    canard::packet{pkt_in.frame()}.ether_header(
        [&](canard::ether_header const& header) {

      auto& fdb = channel->template get_data<learning_switch>();

      fdb[header.source()] = pkt_in.in_port();
      auto const it = fdb.find(header.destination());
      if (it != fdb.end()) {
        flow_add(channel, pkt_in, it->second);
      }
      else {
        flooding(channel, std::move(pkt_in));
      }
    });
  }

  template <class Channel, class Message, class... Args>
  void handle(Channel const&, Message const&, Args&&...)
  {
    static_assert(!std::is_same<Message, v10::messages::packet_in>::value, "");
  }

private:
  template <class Channel>
  void flow_add(Channel const& channel
              , v10::messages::packet_in const& pkt_in
              , std::uint32_t const out_port)
  {
    static thread_local auto cookie = std::uint64_t{0};
    channel->async_send(v10::messages::flow_add{{
          match_from_packet(pkt_in.frame(), pkt_in.in_port())
        , 65535
        , cookie++
        , { v10::actions::output(out_port) }
    }, 0, pkt_in.buffer_id()});
  }

  template <class Channel>
  void flooding(Channel const& channel, v10::messages::packet_in&& pkt_in)
  {
    if (pkt_in.buffer_id() == v10::protocol::OFP_NO_BUFFER) {
      channel->async_send(v10::messages::packet_out{
            pkt_in.extract_frame(), pkt_in.in_port()
          , { v10::actions::output{v10::protocol::OFPP_ALL} }});
    }
    else {
      channel->async_send(v10::messages::packet_out{
            pkt_in.buffer_id(), pkt_in.in_port()
          , { v10::actions::output{v10::protocol::OFPP_ALL} }});
    }
  }
};

int main(int argc, char* argv[])
{
  if (argc != 4) {
    std::cout << "Usage: " << argv[0] << " #io_servs #threads_per_iosrv" << std::endl;
    return -1;
  }

  auto const num_io_service = std::stoul(argv[1]);
  auto const num_thread_per_iosrv = std::stoul(argv[2]);

  using canard::network::utils::io_service_pool;
  auto pool = std::make_shared<io_service_pool>(
      num_io_service, num_thread_per_iosrv);
  learning_switch handler{};

  using controller = allium::controller<learning_switch>;
  controller cont{
    controller::options{handler}
      .address("0.0.0.0").port(argv[3]).io_service_pool(pool)
  };

  try {
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

