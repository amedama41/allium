#ifndef TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
#define TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>

struct table_miss_entry_setting_decorator
{
    template <class Forwarder, class Channel>
    void handle(Forwarder forward
              , Channel const& channel, canard::network::openflow::hello hello)
    {
        namespace v13 = canard::network::openflow::v13;

        channel->async_send(v13::messages::flow_add{{
                  v13::flow_entry_id::table_miss()
                , 0x00000000
                , v13::instructions::write_actions{v13::actions::output::to_controller()}
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
        forward(this, channel, std::move(hello));
    }

    template <class Forwarder, class Channel, class Message>
    void handle(Forwarder forward, Channel const& channel, Message&& message)
    {
        forward(this, channel, std::forward<Message>(message));
    }
};

#endif // TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
