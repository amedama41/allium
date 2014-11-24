#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/overwrite.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    auto const pkt_in_size
        = offsetof(detail::ofp_packet_in, pad) + sizeof(detail::ofp_packet_in::pad);

    auto packet_in_byte_range(detail::ofp_packet_in& pkt_in)
        -> boost::iterator_range<unsigned char*>
    {
        return boost::make_iterator_range(
                  reinterpret_cast<unsigned char*>(&pkt_in)
                , reinterpret_cast<unsigned char*>(&pkt_in) + pkt_in_size);
    }

    template <class Iterator>
    auto decode_packet_in(Iterator& first, Iterator last)
        -> detail::ofp_packet_in
    {
        auto pkt_in = detail::ofp_packet_in{};
        boost::overwrite(
                  boost::make_iterator_range(first, std::next(first, pkt_in_size))
                , packet_in_byte_range(pkt_in));
        std::advance(first, pkt_in_size);
        return detail::ntoh(pkt_in);
    }

    class packet_in
        : public detail::basic_openflow_message<packet_in>
    {
    public:
        static ofp_type const message_type = OFPT_PACKET_IN;

        auto header() const
            -> detail::ofp_header
        {
            return packet_in_.header;
        }

        auto buffer_id() const
            -> std::uint32_t
        {
            return packet_in_.buffer_id;
        }

        auto in_port() const
            -> std::uint16_t
        {
            return packet_in_.in_port;
        }

        auto reason() const
            -> ofp_packet_in_reason
        {
            return ofp_packet_in_reason(packet_in_.reason);
        }

        auto frame() const
            -> std::vector<std::uint8_t>
        {
            return data_;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_in
        {
            auto const pkt_in = decode_packet_in(first, last);
            auto data = std::vector<std::uint8_t>(first, last);
            return packet_in{pkt_in, std::move(data)};
        }

    private:
        packet_in(detail::ofp_packet_in const& pkt_in, std::vector<std::uint8_t> data)
            : packet_in_(pkt_in)
            , data_(std::move(data))
        {
            if (version() != OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
        }

    private:
        detail::ofp_packet_in packet_in_;
        std::vector<std::uint8_t> data_;
    };

} // namespace message
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP
