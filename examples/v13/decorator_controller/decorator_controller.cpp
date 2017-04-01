#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <canard/net/ofp/controller/controller.hpp>
#include <canard/net/ofp/controller/v13/openflow_channel.hpp>
#include "table_miss_entry_setting_decorator.hpp"
#include "logging_decorator.hpp"

namespace allium = canard::net::ofp::controller;
namespace ofp = canard::net::ofp;
namespace v13 = ofp::v13;
namespace msg = v13::messages;

template <class Base>
using cout_logging_decorator = logging_decorator<std::ostream&, Base>;

struct flooding_handler
    : allium::decorate<
            flooding_handler
          , table_miss_entry_setting_decorator, cout_logging_decorator
      >
{
    using versions = std::tuple<allium::v13::version>;

    flooding_handler()
        : decorate{
            allium::make_args<cout_logging_decorator>(std::cout)
          }
    {
    }

    template <class Channel>
    void handle(Channel const& channel, msg::packet_in const& pkt_in)
    {
        channel->async_send(msg::packet_out{
                  pkt_in.frame()
                , v13::protocol::OFPP_CONTROLLER
                , v13::actions::output{v13::protocol::OFPP_ALL}});
    }

    template <class Channel, class... Args>
    void handle(Channel const& channel, Args const&...) {}
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
        return 1;
    }

    flooding_handler handler{};

    using controller = allium::controller<flooding_handler>;
    try {
        controller cont{
            controller::options{handler}.address(argv[1]).port("6653")
        };
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

