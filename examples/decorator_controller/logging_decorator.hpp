#ifndef LOGGING_DECORATOR_HPP
#define LOGGING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>

template <class Logger, class Base = canard::network::openflow::v13::null_decorator>
struct logging_decorator
{
    template <class Derived>
    struct type : canard::network::openflow::v13::decoration<type<Derived>, Base>
    {
        template <class... Args>
        type(Logger const& logger, Args&&... args)
            : type::base_type{std::forward<Args>(args)...}
            , logger(logger)
        {
        }

        template <class Channel>
        void handle(Channel&& channel)
        {
            logger << "connect form " << channel->endpoint() << std::endl;
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel));
        }

        template <class Channel, class Message>
        void handle(Channel&& channel, Message&& message)
        {
            logger << message << std::endl;
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel), std::forward<Message>(message));
        }

        Logger logger;
    };
};

#endif // LOGGING_DECORATOR_HPP
