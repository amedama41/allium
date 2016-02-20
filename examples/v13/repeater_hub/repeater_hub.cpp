#include <iostream>
#include <tuple>
#include <type_traits>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include "../oxm_match_creator.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace msg = v13::messages;
namespace act = v13::actions;
namespace ins = v13::instructions;

struct repeater_hub
{
    using versions = std::tuple<v13::version>;

    template <class Channel>
    void handle(Channel const& channel, of::hello const&)
    {
        channel->async_send(msg::flow_add{{
                  v13::flow_entry_id::table_miss()
                , 0x00000000
                , ins::write_actions{act::output::to_controller()}
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
    }

    template <class Channel>
    void handle(Channel const& channel, msg::packet_in pkt_in)
    {
        channel->async_send(msg::flow_add{{
                  oxm_match_from_packet(pkt_in.frame())
                , 65535
                , 0x00000000
                , ins::write_actions{act::output{v13::protocol::OFPP_ALL}}
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
        channel->async_send(msg::packet_out{
                  pkt_in.extract_frame()
                , v13::protocol::OFPP_CONTROLLER
                , act::output{v13::protocol::OFPP_ALL}
        });
    }

    template <class Channel, class Message, class... Args>
    void handle(Channel const&, Message const&, Args const&...) {
        static_assert(!std::is_same<Message, msg::packet_in>::value
                    , "packet_in message must not be passed to this function");
    }
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

