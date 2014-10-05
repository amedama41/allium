#include <iostream>
#include <canard/network/protocol/openflow/v13.hpp>

namespace of = canard::network::openflow::v13;

struct dump_table_features;
using controller = of::controller<dump_table_features>;

struct dump_table_features
{
    void handle(controller::channel_ptr channel)
    {
        channel->send(of::table_features_request{});
    }

    void handle(controller::channel_ptr channel, of::table_features_reply reply)
    {
        std::cout << reply << std::endl;
    }

    template <class... Args>
    void handle(controller::channel_ptr, Args const&...) {}
};

int main()
{
    auto handler = dump_table_features{};
    controller cont{controller::options{handler}.address("0.0.0.0")};
    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

