#ifndef CANARD_SET_PACKET_IN_ENTRY_DECORATOR_HPP
#define CANARD_SET_PACKET_IN_ENTRY_DECORATOR_HPP

#include <utility>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/message/flow_mod.hpp>
#include <canard/network/protocol/openflow/v13/action/output.hpp>
#include <canard/network/protocol/openflow/v13/instruction/write_actions.hpp>

template <class Base = canard::network::openflow::v13::null_decorator>
class auto_packet_in_decorator
{
public:
    template <class Derived>
    class type : public canard::network::openflow::v13::decoration<type<Derived>, Base>
    {
    public:
        template <class... Args>
        explicit type(Args&&... args)
            : type::base_type{std::forward<Args>(args)...}
        {
        }

        template <class Channel>
        void handle(Channel&& channel)
        {
            namespace of13 = canard::network::openflow::v13;
            channel->send(of13::flow_mod_add{
                    of13::flow_entry{
                          of13::flow_entry_id::table_miss()
                        , of13::instructions::write_actions{of13::actions::output::to_controller()}
                    }, 0, of13::OFPFF_SEND_FLOW_REM});
            static_cast<Derived*>(this)->handle(std::forward<Channel>(channel));
        }

        template <class... Args>
        void handle(Args&&... args)
        {
            static_cast<Derived*>(this)->handle(std::forward<Args>(args)...);
        }
    };
};

#endif // CANARD_SET_PACKET_IN_ENTRY_DECORATOR_HPP
