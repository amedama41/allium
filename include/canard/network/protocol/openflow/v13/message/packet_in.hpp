#ifndef CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <boost/range/iterator_range.hpp>
#include <canard/network/protocol/openflow/binary_data.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class packet_in
        : public v13_detail::basic_openflow_message<packet_in>
    {
        using data_type = std::unique_ptr<unsigned char[]>;

        static constexpr std::uint16_t base_pkt_in_size
            = sizeof(v13_detail::ofp_packet_in) + sizeof(v13_detail::ofp_match);

    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_PACKET_IN;

        packet_in(std::uint32_t const buffer_id
                , std::uint16_t const total_len
                , protocol::ofp_packet_in_reason const reason
                , std::uint8_t const table_id
                , std::uint64_t const cookie
                , oxm_match match
                , binary_data data
                , std::uint32_t const xid = get_xid())
            : packet_in_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(
                              sizeof(v13_detail::ofp_packet_in)
                            + v13_detail::exact_length(match.length())
                            + (data.size() ? 2 + data.size() : 0))
                    , xid
                  }
                , buffer_id
                , total_len
                , std::uint8_t(reason)
                , table_id
                , cookie
              }
            , match_(std::move(match))
            , data_(std::move(data).data())
        {
        }

        template <class Range>
        packet_in(std::uint32_t const buffer_id
                , std::uint16_t const total_len
                , protocol::ofp_packet_in_reason const reason
                , std::uint8_t const table_id
                , std::uint64_t const cookie
                , oxm_match match
                , Range const& data
                , std::uint32_t const xid = get_xid())
            : packet_in{
                  buffer_id, total_len, reason, table_id, cookie
                , std::move(match)
                , binary_data{data}
                , xid
              }
        {
        }

        packet_in(packet_in const& other)
            : packet_in_(other.packet_in_)
            , match_(other.match_)
            , data_{new unsigned char[other.frame_length()]}
        {
            std::copy(other.data_.get()
                    , other.data_.get() + other.frame_length()
                    , data_.get());
        }

        packet_in(packet_in&& other) noexcept
            : packet_in_(other.packet_in_)
            , match_(std::move(other.match_))
            , data_(std::move(other.data_))
        {
            other.packet_in_.header.length = base_pkt_in_size;
        }

        auto operator=(packet_in const& other)
            -> packet_in&
        {
            auto pkt_in = other;
            packet_in_ = pkt_in.packet_in_;
            match_.swap(pkt_in.match_);
            data_.swap(pkt_in.data_);
            return *this;
        }

        auto operator=(packet_in&& other) noexcept
            -> packet_in&
        {
            auto pkt_in = std::move(other);
            packet_in_ = pkt_in.packet_in_;
            match_.swap(pkt_in.match_);
            data_.swap(pkt_in.data_);
            return *this;
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
        {
            return packet_in_.header;
        }

        auto buffer_id() const noexcept
            -> std::uint32_t
        {
            return packet_in_.buffer_id;
        }

        auto total_length() const noexcept
            -> std::uint16_t
        {
            return packet_in_.total_len;
        }

        auto in_port() const
            -> std::uint32_t
        {
            return match().get<oxm_in_port>()->oxm_value();
        }

        auto reason() const noexcept
            -> protocol::ofp_packet_in_reason
        {
            return protocol::ofp_packet_in_reason(packet_in_.reason);
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return packet_in_.table_id;
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return packet_in_.cookie;
        }

        auto match() const noexcept
            -> oxm_match const&
        {
            return match_;
        }

        auto frame() const noexcept
            -> boost::iterator_range<unsigned char const*>
        {
            return boost::make_iterator_range_n(data_.get(), frame_length());
        }

        auto frame_length() const noexcept
            -> std::uint16_t
        {
            auto const padding_data_length
               = length()
               - sizeof(v13_detail::ofp_packet_in)
               - v13_detail::exact_length(match().length());
            return padding_data_length ? padding_data_length - 2 : 0;
        }

        auto extract_frame() noexcept
            -> binary_data
        {
            auto const frame_len = frame_length();
            packet_in_.header.length -= frame_len;
            return binary_data{std::move(data_), frame_len};
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, packet_in_);
            match_.encode(container);
            if (auto const frame_len = frame_length()) {
                detail::encode_byte_array(container, detail::padding, 2);
                return detail::encode_byte_array(
                        container, data_.get(), frame_len);
            }
            else {
                return container;
            }
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_in
        {
            auto const pkt_in
                = detail::decode<v13_detail::ofp_packet_in>(first, last);
            last = std::next(
                      first
                    , pkt_in.header.length - sizeof(v13_detail::ofp_packet_in));

            auto copy_first = first;
            auto const ofp_match
                = detail::decode<v13_detail::ofp_match>(copy_first, last);
            if (ofp_match.type != protocol::OFPMT_OXM) {
                throw std::runtime_error{"match_type is not OFPMT_OXM"};
            }
            if (sizeof(v13_detail::ofp_match) + std::distance(first, last)
                    < ofp_match.length) {
                throw std::runtime_error{"invalid oxm_match length"};
            }

            auto match = oxm_match::decode(first, last);

            if (std::distance(first, last) > 2) {
                std::advance(first, 2);
            }
            auto data = data_type{
                new unsigned char[std::distance(first, last)]
            };
            std::copy(first, last, data.get());
            first = last;

            return packet_in{pkt_in, std::move(match), std::move(data)};
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < base_pkt_in_size) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        packet_in(v13_detail::ofp_packet_in const& pkt_in
                , oxm_match&& match
                , data_type&& data)
            : packet_in_(pkt_in)
            , match_(std::move(match))
            , data_(std::move(data))
        {
        }

    private:
        v13_detail::ofp_packet_in packet_in_;
        oxm_match match_;
        data_type data_;
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PACKET_IN_HPP
