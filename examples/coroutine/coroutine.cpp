#include <chrono>
#include <iostream>
#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include <canard/network/utils/thread_pool.hpp>
#include "../oxm_match_creator.hpp"

namespace of = canard::network::openflow::v13;

struct coroutine_handler;

using controller = of::controller<coroutine_handler>;

struct coroutine_handler
{
    coroutine_handler()
        : io_service_{std::make_shared<boost::asio::io_service>()}
    {
    }

    void handle(controller::channel_ptr channel)
    {
        channel->send(of::flow_mod_add{{
              of::flow_entry_id::table_miss()
            , of::instructions::write_actions{of::actions::output::to_controller()}
        }, 0, of::OFPFF_SEND_FLOW_REM});

        boost::asio::spawn(*io_service_, [=](boost::asio::yield_context yield) {
            auto features_txn = channel->send_request(of::features_request{}, yield);
            features_txn.expires_from_now(std::chrono::seconds{2});
            if (auto reply = features_txn.async_wait(yield)) {
                std::cout << *reply << std::endl;
            }

            auto port_dec_txn = channel->send_request(of::port_description_request{}, yield);
            port_dec_txn.expires_from_now(std::chrono::seconds{2});
            if (auto reply = port_dec_txn.async_wait(yield)) {
                std::cout << *reply << std::endl;
            }
        });
    }

    void handle(controller::channel_ptr channel, of::packet_in pkt_in)
    {
        boost::asio::spawn(*io_service_, [=](boost::asio::yield_context yield) {
            channel->send(of::flow_mod_add{{
                  {oxm_match_from_packet(pkt_in.frame()), 65535}
                , of::instructions::write_actions{of::actions::output{of::OFPP_ALL}}
                , of::timeouts{60, 60}
            }, 0, of::OFPFF_SEND_FLOW_REM});

            auto barrier_txn = channel->send_request(of::barrier_request{}, yield);
            barrier_txn.expires_from_now(std::chrono::seconds{2});
            if (auto reply = barrier_txn.async_wait(yield)) {
                channel->send(of::packet_out{pkt_in.frame()
                    , pkt_in.in_port(), of::actions::output{of::OFPP_TABLE}});
            }
        });
    }

    template <class Message>
    void handle(controller::channel_ptr, Message const& msg) {}

    std::shared_ptr<boost::asio::io_service> io_service_;
};

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }

    auto handler = coroutine_handler{};

    using canard::network::utils::thread_pool;
    controller cont{controller::options{handler}.address(argv[1]).port("6653")
        .thread_pool(std::make_shared<thread_pool>(1, handler.io_service_))};

    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
