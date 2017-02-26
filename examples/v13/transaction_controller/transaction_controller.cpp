#include <chrono>
#include <iostream>
#include <boost/asio/spawn.hpp>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include "./transaction_decorator.hpp"
#include "../oxm_match_creator.hpp"

namespace allium = canard::network::openflow;

struct transaction_controller;

using controller = allium::controller<transaction_controller>;

namespace allium = canard::network::openflow;
namespace msg = ofp::v13::messages;

using boost::format;

struct transaction_controller
  : public allium::decorate<
        transaction_controller
      , canard::allium::decorators::transaction_decorator
    >
{
  using versions = std::tuple<allium::v13::version>;

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
                << format{"switch datapath_id=%|016X|"} % reply.datapath_id()
                << std::endl;
            }
            else {
              std::cout << "recieved error for features request: " << std::endl;
            }
        });
    });
    channel->async_send(msg::set_config{
        ofp::v13::protocol::config_flags::normal, 0xffff
    });
    channel->async_send(msg::flow_add{{
          ofp::v13::flow_entry_id::table_miss()
        , 0x0000000000000000
        , ofp::v13::flow_entry::instructions_type{
            ofp::v13::instructions::write_actions{ ofp::v13::actions::output::to_controller() }
          }
    }, 0});
  }

  template <class Channel>
  void handle(Channel const& channel, msg::packet_in const& pkt_in)
  {
    namespace asio = boost::asio;
    asio::spawn(channel->get_context(), [=](asio::yield_context yield) {
        std::cout << "start packet_in handling" << std::endl;

        std::cout << "sned flow_add" << std::endl;
        auto const flow_add_txn = async_send_request(channel, msg::flow_add{{
                { oxm_match_from_packet(pkt_in.frame()), 65535 }
              , 0x00000000
              , ofp::v13::instruction_set{ofp::v13::instructions::apply_actions{
                  ofp::v13::actions::output{ofp::v13::protocol::port_no::flood}
                }}
            }, 0, ofp::v13::timeouts{0, 0}
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
            , { ofp::v13::actions::output{ofp::v13::protocol::port_no::table} }
        }, yield);
        std::cout << "finish packet_in handling" << std::endl;
    });
  }

  template <class... Args>
  void handle(Args const&...) {}
};

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }

    auto handler = transaction_controller{};
    controller cont{controller::options{handler}.address(argv[1]).port("6653")};
    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

