#include <iostream>
#include <tuple>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v13/openflow_channel.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>

namespace allium = canard::net::ofp::controller;
namespace ofp = canard::net::ofp;
namespace v13 = ofp::v13;

struct dump_table_features
{
    using versions = std::tuple<allium::v13::version>;

    template <class Channel>
    void handle(Channel const& channel, ofp::hello const&)
    {
        channel->async_send(v13::messages::multipart::table_features_request{});
    }

    template <class Channel>
    void handle(Channel const& channel
              , v13::messages::multipart::table_features_reply const& reply)
    {
        std::cout << reply << std::endl;
    }

    template <class Channel, class... Args>
    void handle(Channel const&, Args const&...) {}
};

int main()
{
    auto handler = dump_table_features{};
    using controller = allium::controller<dump_table_features>;
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

