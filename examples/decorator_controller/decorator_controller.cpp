#include <iostream>
#include <memory>
#include <utility>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <canard/network/protocol/openflow/v13.hpp>
#include <canard/network/utils/thread_pool.hpp>
#include "table_miss_entry_setting_decorator.hpp"
#include "logging_decorator.hpp"

namespace of = canard::network::openflow::v13;

struct flooding_handler;

using controller = of::controller<flooding_handler>;

struct flooding_handler
    : of::decoration<flooding_handler, logging_decorator<std::ostream&, table_miss_entry_setting_decorator<>>>
{
    template <class... Args>
    flooding_handler(Args&&... args)
        : decoration{std::forward<Args>(args)...}
    {
    }

    void handle(controller::channel_ptr channel, of::packet_in const& pkt_in)
    {
        channel->send(of::packet_out{pkt_in.frame(), of::OFPP_CONTROLLER, of::actions::output{of::OFPP_ALL}});
    }

    template <class... Args>
    void handle(Args const&...) {}
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <address>" << std::endl;
        return 1;
    }

    flooding_handler handler{std::cout};
    auto options = controller::options{handler};

    try {
        using canard::network::utils::thread_pool;
        controller cont{options.address(argv[1]).port("6653")};
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

