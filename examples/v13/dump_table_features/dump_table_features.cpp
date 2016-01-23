#include <iostream>
#include <canard/network/protocol/openflow/v13.hpp>

namespace of = canard::network::openflow;
namespace v13 = of::v13;

struct dump_table_features
{
    template <class Channel>
    void handle(Channel const& channel, of::hello const&)
    {
        channel->async_send(v13::table_features_request{});
    }

    template <class Channel>
    void handle(Channel const& channel, v13::table_features_reply const& reply)
    {
        std::cout << reply << std::endl;
    }

    template <class Channel, class... Args>
    void handle(Channel const&, Args const&...) {}
};

int main()
{
    auto handler = dump_table_features{};
    using controller = v13::controller<dump_table_features>;
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

