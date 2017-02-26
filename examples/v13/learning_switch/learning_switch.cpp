#include <cstdint>
#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/optional/optional.hpp>
#include <canard/packet_parser.hpp>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include "../oxm_match_creator.hpp"

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v13 = ofp::v13;

class forwarding_db
{
    struct fdb_entry
    {
        std::uint32_t port;
        std::chrono::steady_clock::time_point aging_time;
    };

public:
    void start_age_out_timer(
              boost::asio::io_service::strand strand
            , std::shared_ptr<void> const& channel_ptr)
    {
        timer_.emplace(strand.get_io_service());
        age_out(strand, channel_ptr);
    }

    void stop_age_out_timer()
    {
        timer_ = boost::none;
    }

    void learn(canard::mac_address const& mac, std::uint32_t const port)
    {
        fdb_[mac] = fdb_entry{
            port, std::chrono::steady_clock::now() + std::chrono::seconds{30}
        };
    }

    auto get(canard::mac_address const& mac)
        -> boost::optional<std::uint32_t>
    {
        auto const it = fdb_.find(mac);
        return it != fdb_.end() ? boost::make_optional(it->second.port) : boost::none;
    }

    void age_out(boost::asio::io_service::strand strand
               , std::shared_ptr<void> const& channel_ptr)
    {
        auto const now = std::chrono::steady_clock::now();
        for (auto it = fdb_.begin(), end = fdb_.end(); it != end; ) {
            it = it->second.aging_time <= now ? fdb_.erase(it) : ++it;
        }
        timer_->expires_from_now(std::chrono::seconds{1});
        timer_->async_wait(strand.wrap([=](boost::system::error_code const& ec) {
            if (!ec && timer_) {
                age_out(strand, channel_ptr);
            }
        }));
    }

private:
    std::map<canard::mac_address, fdb_entry> fdb_;
    boost::optional<boost::asio::steady_timer> timer_;
};


class learning_switch
{
public:
    using versions = std::tuple<allium::v13::version>;
    using channel_data = forwarding_db;

    template <class Channel>
    void handle(Channel const& channel, v13::messages::packet_in pkt_in)
    {
        auto const& frame = pkt_in.frame();
        canard::packet{frame}.ether_header(
                [&](canard::ether_header const& header) {
            auto const in_port = pkt_in.in_port();
            auto& fdb = channel->template get_data<learning_switch>();
            fdb.learn(header.source(), in_port);

            if (auto const outport = fdb.get(header.destination())) {
                flow_mod(channel, pkt_in.frame(), outport.get());
                channel->async_send(
                          v13::messages::packet_out{pkt_in.extract_frame()
                        , in_port, v13::actions::output{outport.get()}});
            }
            else {
                channel->async_send(
                          v13::messages::packet_out{pkt_in.extract_frame()
                        , in_port, v13::actions::output{v13::protocol::OFPP_ALL}});
            }
        });
    }

    template <class Channel>
    void handle(Channel const& channel, ofp::hello const&)
    {
        auto& fdb = channel->template get_data<learning_switch>();
        fdb.start_age_out_timer(channel->get_context(), channel);
        channel->async_send(
                v13::messages::flow_add{{
                      v13::flow_entry_id::table_miss()
                    , 0x00000000
                    , v13::flow_entry::instructions_type{
                        v13::instructions::apply_actions{
                            v13::actions::output::to_controller()
                        }
                      }
                }, 0, v13::protocol::OFPFF_SEND_FLOW_REM}
        );
    }

    template <class Channel>
    void handle(Channel const& channel, allium::goodbye const&)
    {
        auto& fdb = channel->template get_data<learning_switch>();
        fdb.stop_age_out_timer();
    }

    template <class Channel, class Message>
    void handle(Channel const&, Message const&)
    {
        static_assert(!std::is_same<Message, v13::messages::packet_in>::value
                    , "packet_in message must not be passed to this function");
    }

private:
    template <class Channel, class Frame>
    void flow_mod(Channel const& channel
            , Frame frame, std::uint32_t const port)
    {
        static thread_local auto cookie = std::uint64_t{0};
        channel->async_send(
              v13::messages::flow_add{{
                    oxm_match_from_packet(frame), 65535
                  , cookie++
                  , v13::flow_entry::instructions_type{
                        v13::instructions::apply_actions{v13::actions::output{port}}
                    }
              }
            , 0, v13::protocol::OFPFF_SEND_FLOW_REM}
        );
    }
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
        return 1;
    }

    using controller = allium::controller<learning_switch>;

    auto io_service = std::make_shared<boost::asio::io_service>();
    learning_switch handler{};
    auto options = controller::options{handler};

    try {
        controller cont{options.address(argv[1]).port("6653")};
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

