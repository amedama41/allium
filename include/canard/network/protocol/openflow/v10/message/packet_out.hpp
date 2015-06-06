#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_OUT_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_OUT_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class packet_out
        : public v10_detail::basic_openflow_message<packet_out>
    {
    public:
        static ofp_type const message_type = OFPT_PACKET_OUT;

    private:
        packet_out(std::uint32_t const buffer_id, std::uint16_t const in_port
                , action_list actions, std::vector<std::uint8_t> data
                , std::uint32_t const xid)
            : packet_out_{
                  {
                        OFP_VERSION
                      , message_type
                      , std::uint16_t(sizeof(v10_detail::ofp_packet_out) + actions.length() + data.size())
                      , xid
                  }
                , buffer_id
                , in_port
                , actions.length()
              }
            , actions_(std::move(actions))
            , data_(std::move(data))
        {
        }

    public:
        packet_out(std::uint32_t const buffer_id, std::uint16_t const in_port
                , action_list actions, std::uint32_t const xid = get_xid())
            : packet_out{buffer_id, in_port, std::move(actions), {}, xid}
        {
        }

        packet_out(std::uint32_t const buffer_id, action_list actions
                , std::uint32_t const xid = get_xid())
            : packet_out{buffer_id, OFPP_NONE, std::move(actions), xid}
        {
        }

        packet_out(std::vector<std::uint8_t> data, std::uint16_t const in_port
                , action_list actions, std::uint32_t const xid = get_xid())
            : packet_out{
                  OFP_NO_BUFFER, in_port
                , std::move(actions), std::move(data), xid
              }
        {
        }

        packet_out(std::vector<std::uint8_t> data, action_list actions
                , std::uint32_t const xid = get_xid())
            : packet_out{std::move(data), OFPP_NONE, std::move(actions), xid}
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return packet_out_.header;
        }

        auto buffer_id() const
            -> std::uint32_t
        {
            return packet_out_.buffer_id;
        }

        auto in_port() const
            -> std::uint16_t
        {
            return packet_out_.in_port;
        }

        auto actions_length() const
            -> std::uint16_t
        {
            return packet_out_.actions_len;
        }

        auto actoins() const
            -> action_list const&
        {
            return actions_;
        }

        auto data() const
            -> std::vector<std::uint8_t> const&
        {
            return data_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_out_);
            actions_.encode(container);
            return container.push_back(data_.data(), data_.size());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_out
        {
            auto const pkt_out
                = detail::decode<v10_detail::ofp_packet_out>(first, last);
            if (pkt_out.actions_len > std::distance(first, last)) {
                throw std::runtime_error{"invalid actions_len"};
            }
            auto const actions_last = std::next(first, pkt_out.actions_len);
            auto actions = action_list::decode(first, actions_last);
            auto data = std::vector<std::uint8_t>(first, last);
            first = last;
            return packet_out{pkt_out, std::move(actions), std::move(data)};
        }

    private:
        packet_out(v10_detail::ofp_packet_out const& pkt_out
                , action_list actions, std::vector<std::uint8_t> data)
            : packet_out_(pkt_out)
            , actions_(std::move(actions))
            , data_(std::move(data))
        {
        }

    private:
        v10_detail::ofp_packet_out packet_out_;
        action_list actions_;
        std::vector<std::uint8_t> data_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_OUT_HPP
