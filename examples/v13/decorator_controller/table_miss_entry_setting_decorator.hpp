#ifndef TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
#define TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/net/ofp/v13/messages.hpp>

template <class Base>
struct table_miss_entry_setting_decorator : public Base
{
    template <class Channel>
    void handle(Channel const& channel, canard::net::ofp::hello hello)
    {
        namespace v13 = canard::net::ofp::v13;

        channel->async_send(v13::messages::flow_add{{
                  v13::flow_entry_id::table_miss()
                , 0x00000000
                , v13::flow_entry::instructions_type{
                    v13::instructions::write_actions{v13::actions::output::to_controller()}
                  }
        }, 0, v13::protocol::OFPFF_SEND_FLOW_REM});
        this->forward(channel, std::move(hello));
    }

    template <class Channel, class Message>
    void handle(Channel const& channel, Message&& message)
    {
        this->forward(channel, std::forward<Message>(message));
    }
};

#endif // TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
