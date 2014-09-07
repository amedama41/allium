#include <canard/network/protocol/openflow/v13.hpp>
#include <iostream>
#include <functional>

namespace of = canard::network::openflow::v13;

class transaction_controller;

using controller = of::controller<transaction_controller>;

class transaction_controller
{
public:
    template <class Message>
    void handle_txn(of::transaction<Message> txn
            , boost::system::error_code const& ec, boost::optional<Message> reply)
    {
        if (ec) {
            std::cerr << "can not receive table_features reply because of error: " << ec.message() << std::endl;
            return;
        }
        std::cout << "receive transaction reply: " << *reply << std::endl;
    }

    void handle(controller::channel_ptr channel)
    {
        auto request = of::features_request{};
        channel->send_request(request
                , [=](boost::system::error_code const& ec, of::transaction<of::features_reply> txn)
        {
            if (ec) {
                std::cerr << "send error: " << ec.message() << std::endl;
                return;
            }
            std::cout << "send table features request xid=" << request.xid() << std::endl;
            txn.expires_from_now(std::chrono::seconds{2});
            using namespace std::placeholders;
            txn.async_wait(std::bind(&transaction_controller::handle_txn<of::features_reply>, this, txn, _1, _2));
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

