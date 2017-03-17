#include <chrono>
#include <iostream>
#include <boost/asio/spawn.hpp>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>
#include "./transaction_decorator.hpp"
#include "../match_creator.hpp"

namespace allium = canard::net::ofp::controller;
namespace msg = ofp::v10::messages;

using boost::format;

struct transaction_switch
  : public allium::decorate<
        transaction_switch
      , canard::allium::decorators::transaction_decorator
    >
{
  using versions = std::tuple<allium::v10::version>;

  template <class Channel>
  void handle(Channel const& channel, ofp::hello const& hello)
  {
    using txn_ptr = transaction_ptr<msg::features_request>;

    std::cout << "send features_request" << std::endl;
    async_send_request(
          channel, msg::features_request{}
        , [](boost::system::error_code ec, txn_ptr txn) {
        if (ec) {
          std::cout
            << "features_request send failed: " << ec.message() << std::endl;
          return;
        }

        std::cout << "wait for features_reply..." << std::endl;
        async_receive_response(
            txn, [txn](boost::system::error_code ec
                     , txn_ptr::element_type::message_type msg) {
            if (ec) {
              std::cout << "some error occurred: " << ec.message() << std::endl;
              return;
            }
            if (msg.is_reply()) {
              std::cout << "received features reply" << std::endl;
              auto const& reply = msg.reply();
              std::cout
                << format{"switch datapath_id=%|016X|, number of ports=%||"}
                    % reply.datapath_id() % reply.ports().size()
                << std::endl;
            }
            else {
              std::cout << "recieved error for features request: " << std::endl;
            }
        });
    });
    channel->async_send(ofp::v10::messages::set_config{
        ofp::v10::protocol::config_flags::normal, 0xffff
    });
  }

  template <class Channel>
  void handle(Channel const& channel, msg::packet_in const& pkt_in)
  {
    namespace asio = boost::asio;
    asio::spawn(channel->get_context(), [=](asio::yield_context yield) {
        std::cout << "start packet_in handling" << std::endl;

        std::cout << "sned flow_add" << std::endl;
        auto const flow_add_txn = async_send_request(channel, msg::flow_add{
                match_from_packet(pkt_in.frame(), pkt_in.in_port()), 65535
              , 0x00000000
              , { ofp::v10::actions::output{ofp::v10::protocol::port_no::flood} }
              , ofp::v10::timeouts{0, 0}
        }, yield);

        std::cout << "send barrier_request" << std::endl;
        auto const barrier_txn
          = async_send_request(channel, msg::barrier_request{}, yield);

        auto const txn_msg
          = async_receive_response(barrier_txn, std::chrono::seconds{1}, yield);
        if (!txn_msg.is_reply()) {
          std::cout
            << "timeout or received error for barrier_request: " << std::endl;
          return;
        }
        std::cout << "received barrier_reply" << std::endl;

        if (auto const error = async_receive_response(
              flow_add_txn, std::chrono::seconds{0}, yield)) {
          std::cout
            << format{"received error for flow_add: type=%||, code=%||"}
               % error->error_type() % error->error_code()
            << std::endl;
          return;
        }

        std::cout << "no error for flow_add, then send packet_out" << std::endl;
        channel->async_send(msg::packet_out{
              pkt_in.frame(), pkt_in.in_port()
            , { ofp::v10::actions::output{ofp::v10::protocol::port_no::table} }
        }, yield);
        std::cout << "finish packet_in handling" << std::endl;
    });
  }

  template <class... Args>
  void handle(Args const&...) {}
};

int main()
{
  auto handler = transaction_switch{};
  using controller = allium::controller<transaction_switch>;
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

