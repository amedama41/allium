#include <iostream>
#include <tuple>
#include <type_traits>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include "../oxm_match_creator.hpp"

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v13 = ofp::v13;

struct repeater_hub
{
    using versions = std::tuple<allium::v13::version>;

    template <class Channel>
    void handle(Channel const& channel, ofp::hello const&)
    {
        channel->async_send(v13::messages::flow_add{{
                  v13::flow_entry_id::table_miss()
                , 0x00000000
                , v13::flow_entry::instructions_type{
                    v13::instructions::write_actions{v13::actions::output::to_controller()}
                  }
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
    }

    template <class Channel>
    void handle(Channel const& channel, v13::messages::packet_in pkt_in)
    {
        channel->async_send(v13::messages::flow_add{{
                  oxm_match_from_packet(pkt_in.frame())
                , 65535
                , 0x00000000
                , v13::flow_entry::instructions_type{
                    v13::instructions::write_actions{v13::actions::output{v13::protocol::OFPP_ALL}}
                  }
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
        channel->async_send(v13::messages::packet_out{
                  pkt_in.extract_frame()
                , v13::protocol::OFPP_CONTROLLER
                , v13::actions::output{v13::protocol::OFPP_ALL}
        });
    }

    template <class Channel, class Message, class... Args>
    void handle(Channel const&, Message const&, Args const&...) {
        static_assert(!std::is_same<Message, v13::messages::packet_in>::value
                    , "packet_in message must not be passed to this function");
    }
};

int main()
{
    auto handler = repeater_hub{};
    using controller = allium::controller<repeater_hub>;
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

