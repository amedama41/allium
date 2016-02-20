#include <iostream>
#include <tuple>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13.hpp>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace msg = v13::messages;

struct dump_table_features
{
    using versions = std::tuple<v13::version>;

    template <class Channel>
    void handle(Channel const& channel, of::hello const&)
    {
        channel->async_send(msg::multipart::table_features_request{});
    }

    template <class Channel>
    void handle(Channel const& channel, msg::multipart::table_features_reply const& reply)
    {
        std::cout << reply << std::endl;
    }

    template <class Channel, class... Args>
    void handle(Channel const&, Args const&...) {}
};

int main()
{
    auto handler = dump_table_features{};
    using controller = of::controller<dump_table_features>;
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

