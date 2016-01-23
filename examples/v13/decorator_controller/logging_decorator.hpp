#ifndef LOGGING_DECORATOR_HPP
#define LOGGING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>

template <class Logger>
struct logging_decorator
{
    logging_decorator(Logger&& logger)
        : logger(std::forward<Logger>(logger))
    {
    }

    template <class Forwarder, class Channel>
    void handle(Forwarder forward
              , Channel const& channel, canard::network::openflow::hello hello)
    {
        logger << "connected " << channel.get() << ": " << hello << std::endl;
        forward(this, channel, std::move(hello));
    }

    template <class Forwarder, class Channel>
    void handle(Forwarder forward
              , Channel const& channel, canard::network::openflow::goodbye bye)
    {
        logger << "disconnected " << channel.get() << ": " << bye.reason().message() << std::endl;
        forward(this, channel, std::move(bye));
    }

    template <class Forwarder, class Channel, class Message>
    void handle(Forwarder forward, Channel const& channel, Message&& message)
    {
        logger << message << std::endl;
        forward(this, channel, std::forward<Message>(message));
    }

    Logger logger;
};

#endif // LOGGING_DECORATOR_HPP
