#include <chrono>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include "../oxm_match_creator.hpp"
#include "../transaction_controller/transaction_decorator.hpp"

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v13 = ofp::v13;
namespace msg = v13::messages;
namespace protocol = v13::protocol;
namespace act = v13::actions;

struct coroutine_handler;

using controller = allium::controller<coroutine_handler>;

struct coroutine_handler
  : allium::decorate<
      coroutine_handler, canard::allium::decorators::transaction_decorator
    >
{
  using versions = std::tuple<allium::v13::version>;

  template <class Channel>
  void handle(Channel channel, ofp::hello const&)
  {
    channel->async_send(msg::flow_add{{
          v13::flow_entry_id::table_miss()
        , 0x0000000000000000
        , v13::flow_entry::instructions_type{
            v13::instructions::write_actions{ act::output::to_controller() }
          }
    }, 0, protocol::OFPFF_SEND_FLOW_REM});

    boost::asio::spawn(
        channel->get_context(), [=](boost::asio::yield_context yield) {
        auto const features_txn
          = async_send_request(channel, msg::features_request{}, yield);
        auto const features_response = async_receive_response(
            features_txn, std::chrono::seconds{2}, yield);
        if (!features_response.is_reply()) {
          std::cout << "no features reply" << std::endl;
          return;
        }
        dump(features_response.reply());

        auto const port_desc_txn = async_send_request(
            channel, msg::multipart::port_description_request{}, yield);
        auto const port_desc_response = async_receive_response(
            port_desc_txn, std::chrono::seconds{2}, yield);
        if (!port_desc_response.is_reply()) {
          std::cout << "no port_description reply" << std::endl;
          return;
        }
        dump(port_desc_response.reply());
    });
  }

  template <class Channel>
  void handle(Channel channel, msg::packet_in pkt_in)
  {
    boost::asio::spawn(
        channel->get_context(), [=](boost::asio::yield_context yield) {
        channel->async_send(msg::flow_add{{
              {oxm_match_from_packet(pkt_in.frame()), 65535}
            , 0x0000000000000000
            , v13::flow_entry::instructions_type{
                v13::instructions::write_actions{ act::output{protocol::OFPP_ALL} }
              }
        }, 0, v13::timeouts{60, 60}, protocol::OFPFF_SEND_FLOW_REM});

        auto const barrier_txn
          = async_send_request(channel, msg::barrier_request{}, yield);
        auto const barrier_response = async_receive_response(
            barrier_txn, std::chrono::seconds{2}, yield);
        if (barrier_response.is_reply()) {
            channel->async_send(msg::packet_out{
                  pkt_in.frame()
                , pkt_in.in_port(), act::output{protocol::OFPP_TABLE}});
        }
    });
  }

  template <class Channel, class Message>
  void handle(Channel, Message const& msg) {}

  void dump(msg::features_reply const& reply) const
  {
    std::cout
      << (boost::format{
          "FeaturesReply: datapath_id=0x%|016x|,n_buffers=%||,n_tables=%||"
          ",auxiliary_id=%||,capabilities=0x%|08x|"
          }
          % reply.datapath_id()
          % reply.num_buffers()
          % int(reply.num_tables())
          % int(reply.auxiliary_id())
          % reply.capabilities())
      << std::endl;
  }
  void dump(msg::multipart::port_description_reply const& reply) const
  {
    std::cout << "PortDesciptionReply:" << std::endl;
    for (auto const& port : reply.body()) {
      std::cout
        << (boost::format{
            " Port: port_no=%||,hw_addr=%||,name=%||,config=0x%|08x|,state=0x%|08x|"
            ",curr=0x%|08x|,advertised=0x%|08x|,supported=0x%|08x|,peer=0x%|08x|"
            ",curr_speed=%||,max_speed=%||"
            }
            % port.port_no()
            % port.hardware_address()
            % port.name()
            % port.config()
            % port.state()
            % port.current_features()
            % port.advertised_features()
            % port.supported_features()
            % port.peer_advertised_features()
            % port.current_speed()
            % port.max_speed())
        << std::endl;
    }
  }
};

int main(int argc, char const* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
    return 1;
  }

  auto handler = coroutine_handler{};

  controller cont{
    controller::options{handler}.address(argv[1]).port("6653")
  };

  try {
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
