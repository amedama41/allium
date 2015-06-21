#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <canard/packet_parser.hpp>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include <canard/network/utils/thread_pool.hpp>
#include "../oxm_match_creator.hpp"

namespace of = canard::network::openflow::v13;

class forwarding_db
{
    struct fdb_entry
    {
        std::uint32_t port;
        std::chrono::steady_clock::time_point aging_time;
    };

public:
    forwarding_db(boost::asio::io_service& io_service)
        : fdb_{}
        , timer_{io_service, std::chrono::seconds{1}}
    {
        timer_.async_wait([=](boost::system::error_code) {
            age_out();
        });
    }

    void learn(canard::mac_address const& mac, std::uint32_t const port)
    {
        fdb_[mac] = fdb_entry{port, std::chrono::steady_clock::now() + std::chrono::seconds{30}};
    }

    auto get(canard::mac_address const& mac)
        -> boost::optional<std::uint32_t>
    {
        auto const it = fdb_.find(mac);
        return it != fdb_.end() ? boost::make_optional(it->second.port) : boost::none;
    }

    void age_out()
    {
        auto const now = std::chrono::steady_clock::now();
        for (auto it = fdb_.begin(), end = fdb_.end(); it != end; ) {
            it = it->second.aging_time <= now ? fdb_.erase(it) : ++it;
        }
        timer_.expires_from_now(std::chrono::seconds{1});
        timer_.async_wait([=](boost::system::error_code) {
            age_out();
        });
    }

private:
    std::map<canard::mac_address, fdb_entry> fdb_;
    boost::asio::steady_timer timer_;
};


class learning_switch;
using controller = of::controller<learning_switch>;

class learning_switch
{
public:
    learning_switch(boost::asio::io_service& io_service)
        : fdb_{io_service}
    {
    }

    void handle(controller::channel_ptr channel, of::packet_in const& pkt_in)
    {
        canard::packet{pkt_in.frame()}.ether_header([&](canard::ether_header const& header) {
            auto const in_port = pkt_in.in_port();
            fdb_.learn(header.source(), in_port);

            if (auto const outport = fdb_.get(header.destination())) {
                flow_mod(channel, pkt_in.frame(), outport.get());
                channel->send(of::packet_out{pkt_in.frame(), in_port, of::actions::output{outport.get()}});
            }
            else {
                channel->send(of::packet_out{pkt_in.frame(), in_port, of::actions::output{of::protocol::OFPP_ALL}});
            }
        });
    }

    void handle(controller::channel_ptr channel)
    {
        channel->send(
                of::flow_mod_add{{
                      of::flow_entry_id::table_miss()
                    , of::instructions::apply_actions{of::actions::output::to_controller()}
                }, 0, of::protocol::OFPFF_SEND_FLOW_REM}
        );
    }

    template <class Message>
    void handle(controller::channel_ptr, Message const&) {}

private:
    void flow_mod(controller::channel_ptr const& channel
            , std::vector<std::uint8_t> const& frame, std::uint32_t const port)
    {
        channel->send(
              of::flow_mod_add{{
                    {oxm_match_from_packet(frame), 65535}
                  , of::instructions::apply_actions{of::actions::output{port}}
              }
            , 0, of::protocol::OFPFF_SEND_FLOW_REM}
        );
    }

private:
    forwarding_db fdb_;
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
        return 1;
    }

    auto io_service = std::make_shared<boost::asio::io_service>();
    learning_switch handler{*io_service};
    auto options = controller::options{handler};

    try {
        using canard::network::utils::thread_pool;
        controller cont{options.address(argv[1]).port("6653")
            .thread_pool(std::make_shared<thread_pool>(1, io_service))};
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

