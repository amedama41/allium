#ifndef CANARD_NETWORK_OPENFLOW_V13_NEGOTIATION_DECORATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_NEGOTIATION_DECORATOR_HPP

#include <chrono>
#include <map>
#include <utility>
#include <boost/asio/ip/tcp.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/disconnected_info.hpp>
#include <canard/network/protocol/openflow/v13/message/switch_features.hpp>
#include <canard/network/protocol/openflow/v13/transaction.hpp>

template <class Base = canard::network::openflow::v13::null_decorator>
class negotiation_decorator
{
public:
    template <class Derived>
    class type : public canard::network::openflow::v13::decoration<type<Derived>, Base>
    {
        using features_reply = canard::network::openflow::v13::features_reply;

    public:
        template <class... Args>
        explicit type(Args&&... args)
            : type::base_type{std::forward<Args>(args)...}
        {
        }

        template <class Channel>
        auto get_switch_features(Channel const& channel) const
            -> features_reply const&
        {
            return sw_features_.find(channel->endpoint())->second;
        }

        template <class Channel>
        auto get_datapath_id(Channel const& channel) const
            -> std::uint64_t
        {
            return get_switch_features(channel).datapath_id();
        }


        template <class Channel>
        void handle(Channel&& channel)
        {
            namespace of13 = canard::network::openflow::v13;

            channel->send_request(of13::features_request{}
                    , [=](boost::system::error_code const& ec, of13::transaction<features_reply> txn)
            {
                if (ec) {
                    BOOST_LOG_TRIVIAL(info)
                        << boost::format{"sening features request to %s failed due to %s"}
                        % channel->endpoint() % ec.message();
                    return;
                }
                txn.expires_from_now(std::chrono::seconds{10});
                txn.async_wait([=](boost::system::error_code const& ec, boost::optional<features_reply> reply)
                {
                    if (ec) {
                        BOOST_LOG_TRIVIAL(info)
                            << boost::format{"receiving features reply from %s failed due to %s"}
                            % channel->endpoint() % ec.message();
                        return;
                    }
                    sw_features_.emplace(channel->endpoint(), std::move(*reply));
                    static_cast<Derived*>(this)->handle(channel);
                });
            });
        }

        template <class Channel>
        void handle(Channel&& channel, canard::network::openflow::v13::disconnected_info&& info)
        {
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel), info);
            sw_features_.erase(channel->endpoint());
        }

        template <class... Args>
        void handle(Args&&... args)
        {
            static_cast<Derived*>(this)->handle(std::forward<Args>(args)...);
        }

    private:
        std::map<boost::asio::ip::tcp::endpoint, features_reply> sw_features_;
    };
};

#endif // CANARD_NETWORK_OPENFLOW_V13_NEGOTIATION_DECORATOR_HPP
