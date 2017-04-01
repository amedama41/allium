#define BOOST_LOG_DYN_LINK
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <canard/net/utils/thread_pool.hpp>
#include <canard/net/ofp/controller/v13/decorator/topology/topology_decorator.hpp>

namespace of = canard::network::openflow::v13;

struct my_topology;

using controller = of::controller<my_topology>;

struct my_topology
    : public of::decoration<my_topology, topology::topology_decorator<>>
{
    explicit my_topology(boost::asio::io_service& io_service)
        : base_type(io_service)
    {
    }

    template <class ChannelPtr>
    void handle(ChannelPtr&& channel)
    {
        BOOST_LOG_TRIVIAL(info) << boost::format{"connect from 0x%x"}
            % get_datapath_id(channel);
    }

    template <class ChannelPtr>
    void handle(ChannelPtr&& channel, of::disconnected_info&& info)
    {
        BOOST_LOG_TRIVIAL(info) << boost::format{"disconnect from 0x%x due to %s"}
            % this->get_datapath_id(channel) % info.error().message();
    }

    template <class ChannelPtr, class Message>
    void handle(ChannelPtr&& channel, Message&& msg)
    {
        BOOST_LOG_TRIVIAL(debug) << boost::format{"%s"} % msg;
    }

    void handle(topology::up_link_info&& up_link)
    {
        BOOST_LOG_TRIVIAL(debug) << up_link;
    }

    void handle(topology::down_link_info&& down_link)
    {
        BOOST_LOG_TRIVIAL(debug) << down_link;
    }
};

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }

    auto io_service = std::make_shared<boost::asio::io_service>();
    auto handler = my_topology{*io_service};

    using canard::network::utils::thread_pool;
    controller cont{
        controller::options{handler}
            .address(argv[1]).port("6653")
            .thread_pool(std::make_shared<thread_pool>(1, io_service))
    };

    try {
        cont.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

