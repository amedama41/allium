#ifndef CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class packet_in
        : public v13_detail::basic_openflow_message<packet_in>
    {
    public:
        static ofp_type const message_type = OFPT_PACKET_IN;

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return packet_in_.header;
        }

        auto buffer_id() const
            -> std::uint32_t
        {
            return packet_in_.buffer_id;
        }

        auto total_length() const
            -> std::uint32_t
        {
            return packet_in_.total_len;
        }

        auto reason() const
            -> ofp_packet_in_reason
        {
            return ofp_packet_in_reason(packet_in_.reason);
        }

        auto table_id() const
            -> std::uint8_t
        {
            return packet_in_.table_id;
        }

        auto cookie() const
            -> std::uint64_t
        {
            return packet_in_.cookie;
        }

        auto match() const
            -> oxm_match const&
        {
            return match_;
        }

        auto frame() const
            -> std::vector<unsigned char> const&
        {
            return frame_;
        }

        auto in_port() const
            -> std::uint32_t
        {
            return match().get<oxm_in_port>()->oxm_value();
        }

        using basic_openflow_message<packet_in>::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_in_);
            match_.encode(container);
            return frame_.empty()
                ? container
                : boost::push_back(boost::push_back(container, {0, 0}), frame_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_in
        {
            auto const pkt_in = detail::decode<v13_detail::ofp_packet_in>(first, last);
            if (std::distance(first, last) != pkt_in.header.length - sizeof(v13_detail::ofp_packet_in)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);

            if (std::distance(first, last) > 2) {
                std::advance(first, 2);
            }
            auto frame = std::vector<unsigned char>(first, last);
            std::advance(first, frame.size());

            return {pkt_in, std::move(match), std::move(frame)};
        }

    private:
        packet_in(v13_detail::ofp_packet_in const& pkt_in, oxm_match match, std::vector<unsigned char> frame)
            : packet_in_(pkt_in)
            , match_(std::move(match))
            , frame_(std::move(frame))
        {
        }

    private:
        v13_detail::ofp_packet_in packet_in_;
        oxm_match match_;
        std::vector<unsigned char> frame_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP
