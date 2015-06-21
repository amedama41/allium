#ifndef CANARD_NETWORK_OPENFLOW_V13_PACKET_OUT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PACKET_OUT_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>


namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class packet_out
        : public v13_detail::basic_openflow_message<packet_out>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_PACKET_OUT;

        template <class... Actions>
        packet_out(std::vector<std::uint8_t> data, std::uint32_t const in_port, Actions&&... actions)
            : packet_out_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , std::uint16_t(sizeof(v13_detail::ofp_packet_out) + actions_length(actions...) + data.size())
                    , get_xid()
                  }
                , protocol::OFP_NO_BUFFER, in_port, actions_length(actions...), {0}
              }
            , action_list_{std::forward<Actions>(actions)...}
            , data_(std::move(data))
        {
        }

        template <class... Actions>
        packet_out(std::uint32_t const buffer_id, std::uint32_t const in_port, Actions&&... actions)
            : packet_out_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , std::uint16_t(sizeof(v13_detail::ofp_packet_out) + actions_length(actions...))
                    , get_xid()
                  }
                , buffer_id, in_port, actions_length(actions...), {0}
              }
            , action_list_{std::forward<Actions>(actions)...}
            , data_{}
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return packet_out_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_out_);
            action_list_.encode(container);
            return boost::push_back(container, data_);
        }

    private:
        packet_out(v13_detail::ofp_packet_out const& pkt_out, action_list list, std::vector<std::uint8_t> data)
            : packet_out_(pkt_out)
            , action_list_(std::move(list))
            , data_(std::move(data))
        {
        }

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator& last)
            -> packet_out
        {
            auto const pkt_out = detail::decode<v13_detail::ofp_packet_out>(first, last);
            if (std::distance(first, last) < std::max<std::uint16_t>(pkt_out.actions_len, pkt_out.header.length - sizeof(v13_detail::ofp_packet_out))) {
                throw 1;
            }
            auto act_list = action_list::decode(first, std::next(first, pkt_out.actions_len));
            auto data = std::vector<std::uint8_t>(first, last);
            std::advance(first, last);
            return packet_out{pkt_out, std::move(act_list), std::move(data)};
        }

    private:
        static auto actions_length()
            -> std::uint16_t
        {
            return 0;
        }

        template <class Action, class... Actions>
        static auto actions_length(Action const& action, Actions const&... actions)
            -> std::uint16_t
        {
            return action.length() + actions_length(actions...);
        }

    private:
        v13_detail::ofp_packet_out packet_out_;
        action_list action_list_;
        std::vector<std::uint8_t> data_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PACKET_OUT_HPP
