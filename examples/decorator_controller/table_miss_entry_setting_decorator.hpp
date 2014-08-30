#ifndef TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
#define TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>

template <class Base = canard::network::openflow::v13::null_decorator>
struct table_miss_entry_setting_decorator
{
    template <class Derived>
    struct type : canard::network::openflow::v13::decoration<type<Derived>, Base>
    {
        template <class Channel>
        void handle(Channel&& channel)
        {
            namespace of = canard::network::openflow::v13;

            channel->send(of::flow_mod_add{{
                      of::flow_entry_id::table_miss()
                    , of::instructions::write_actions{of::actions::output::to_controller()}
            }, 0, of::OFPFF_SEND_FLOW_REM});
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel));
        }

        template <class Channel, class Message>
        void handle(Channel&& channel, Message&& message)
        {
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel), std::forward<Message>(message));
        }
    };
};

#endif // TABLE_MISS_ENTRY_SETTING_DECORATOR_HPP
