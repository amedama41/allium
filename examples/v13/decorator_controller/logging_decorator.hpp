#ifndef LOGGING_DECORATOR_HPP
#define LOGGING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/v13/io/openflow_io.hpp>

template <class Logger, class Base>
struct logging_decorator : public Base
{
    logging_decorator(Logger&& logger)
        : logger(std::forward<Logger>(logger))
    {
    }

    template <class Channel>
    void handle(Channel const& channel, canard::net::ofp::hello hello)
    {
        logger << "connected " << channel.get() << ": " << hello << std::endl;
        this->forward(channel, std::move(hello));
    }

    template <class Channel>
    void handle(Channel const& channel, canard::net::ofp::controller::goodbye bye)
    {
        logger << "disconnected " << channel.get() << ": " << bye.reason().message() << std::endl;
        this->forward(channel, std::move(bye));
    }

    template <class Channel, class Message>
    void handle(Channel const& channel, Message&& message)
    {
        logger << message << std::endl;
        this->forward(channel, std::forward<Message>(message));
    }

    Logger logger;
};

#endif // LOGGING_DECORATOR_HPP
