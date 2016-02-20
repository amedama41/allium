#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include "table_miss_entry_setting_decorator.hpp"
#include "logging_decorator.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace msg = v13::messages;

struct flooding_handler
    : of::decorate<
          table_miss_entry_setting_decorator
        , logging_decorator<std::ostream&>
      >
{
    using versions = std::tuple<v13::version>;

    flooding_handler()
        : decorate{
            of::make_args<logging_decorator>(std::cout)
          }
    {
    }

    template <class Channel>
    void handle(Channel const& channel, msg::packet_in const& pkt_in)
    {
        channel->async_send(msg::packet_out{
                  of::binary_data{pkt_in.frame()}
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

    using controller = of::controller<flooding_handler>;
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

