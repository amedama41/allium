#include <iostream>
#include <tuple>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include "../oxm_match_creator.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;

struct repeater_hub
{
    using versions = std::tuple<v13::version>;

    template <class Channel>
    void handle(Channel const& channel, of::hello const&)
    {
        channel->async_send(v13::flow_mod_add{{
                  v13::flow_entry_id::table_miss()
                , v13::instructions::write_actions{v13::actions::output::to_controller()}
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
    }

    template <class Channel>
    void handle(Channel const& channel, v13::packet_in pkt_in)
    {
        channel->async_send(v13::flow_mod_add{{
                  {oxm_match_from_packet(pkt_in.frame()), 65535}
                , v13::instructions::write_actions{v13::actions::output{v13::protocol::OFPP_ALL}}
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
        channel->async_send(v13::packet_out{pkt_in.frame()
                , v13::protocol::OFPP_CONTROLLER, v13::actions::output{v13::protocol::OFPP_ALL}});
    }

    template <class Channel, class... Args>
    void handle(Channel const&, Args const&...) {}
};

int main()
{
    auto handler = repeater_hub{};
    using controller = of::controller<repeater_hub>;
    try {
        controller cont{
            controller::options{handler}.address("0.0.0.0")
        };
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

