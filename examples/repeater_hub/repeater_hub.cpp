#include <iostream>
#include <canard/network/protocol/openflow/v13.hpp>
#include "../oxm_match_creator.hpp"

namespace of = canard::network::openflow::v13;

struct repeater_hub;
using controller = of::controller<repeater_hub>;

struct repeater_hub
{
    void handle(controller::channel_ptr channel)
    {
        channel->send(of::flow_mod_add{{
                  of::flow_entry_id::table_miss()
                , of::instructions::write_actions{of::actions::output::to_controller()}
        }, 0, of::OFPFF_SEND_FLOW_REM});
    }

    void handle(controller::channel_ptr channel, of::packet_in pkt_in)
    {
        channel->send(of::flow_mod_add{{
                  {oxm_match_from_packet(pkt_in.frame()), 65535}
                , of::instructions::write_actions{of::actions::output{of::OFPP_ALL}}
        }, 0, of::OFPFF_SEND_FLOW_REM});
        channel->send(of::packet_out{pkt_in.frame()
                , of::OFPP_CONTROLLER, of::actions::output{of::OFPP_ALL}});
    }

    template <class... Args>
    void handle(Args const&...) {}
};

int main()
{
    auto handler = repeater_hub{};
    controller cont{controller::options{handler}.address("0.0.0.0")};
    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

