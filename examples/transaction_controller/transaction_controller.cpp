#include <canard/network/protocol/openflow/v13.hpp>

namespace of = canard::network::openflow::v13;

class transaction_controller;

using controller = of::controller<transaction_controller>;

class transaction_controller
{
public:
    void handle(controller::channel_ptr channel)
    {
        auto request = of::features_request{};
        channel->send_request(request
                , [=](boost::system::error_code const& ec, std::shared_ptr<of::reply_message<of::features_reply>> reply)
        {
            if (ec) {
                std::cerr << "send error: " << ec.message() << std::endl;
                return;
            }
            std::cout << "send request xid=" << request.xid() << std::endl;
            reply->expires_from_now(std::chrono::seconds{2});
            reply->async_wait([](boost::system::error_code const ec, boost::optional<of::features_reply> reply) {
                if (ec) {
                    std::cerr << "can not receive table_features reply because of error: " << ec.message() << std::endl;
                    return;
                }
                std::cout << "receive reply: " << *reply << std::endl;
            });
        });
    }

    template <class Handler>
    void handle(controller::channel_ptr channel, Handler const&)
    {
    }
};

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }

    auto handler = transaction_controller{};
    controller cont{controller::options{handler}.address(argv[1]).port("6653")};
    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

