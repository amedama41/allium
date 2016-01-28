#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP

#include <cstddef>
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
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class packet_in
        : public v10_detail::basic_openflow_message<packet_in>
    {
        using data_type = std::unique_ptr<unsigned char[]>;

        static constexpr std::uint16_t base_pkt_in_size
            = offsetof(v10_detail::ofp_packet_in, pad)
            + sizeof(v10_detail::ofp_packet_in::pad);

    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_PACKET_IN;

        packet_in(std::uint32_t const buffer_id
                , std::uint16_t const total_len
                , std::uint16_t const in_port
                , protocol::ofp_packet_in_reason const reason
                , std::uint8_t const table_id
                , binary_data data
                , std::uint32_t const xid = get_xid())
            : packet_in_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(base_pkt_in_size + data.size())
                    , xid
                  }
                , buffer_id
                , total_len
                , in_port
                , std::uint8_t(reason)
                , 0
              }
            , data_(std::move(data).data())
        {
        }

        template <class Range>
        packet_in(std::uint32_t const buffer_id
                , std::uint16_t const total_len
                , std::uint16_t const in_port
                , protocol::ofp_packet_in_reason const reason
                , std::uint8_t const table_id
                , Range const& data
                , std::uint32_t const xid = get_xid())
            : packet_in{
                  buffer_id, total_len, in_port, reason, table_id
                , binary_data{data}
                , xid
              }
        {
        }

        packet_in(packet_in const& other)
            : packet_in_(other.packet_in_)
            , data_{
                  other.frame_length()
                ? new unsigned char[other.frame_length()]
                : nullptr
              }
        {
            std::copy(other.data_.get()
                    , other.data_.get() + other.frame_length()
                    , data_.get());
        }

        packet_in(packet_in&& other) noexcept
            : packet_in_(other.packet_in_)
            , data_(std::move(other.data_))
        {
            other.packet_in_.header.length = base_pkt_in_size;
        }

        auto operator=(packet_in const& other)
            -> packet_in&
        {
            auto pkt_in = other;
            packet_in_ = pkt_in.packet_in_;
            data_.swap(pkt_in.data_);
            return *this;
        }

        auto operator=(packet_in&& other) noexcept
            -> packet_in&
        {
            auto pkt_in = std::move(other);
            packet_in_ = pkt_in.packet_in_;
            data_.swap(pkt_in.data_);
            return *this;
        }

        auto header() const noexcept
            -> v10_detail::ofp_header const&
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

        auto in_port() const noexcept
            -> std::uint16_t
        {
            return packet_in_.in_port;
        }

        auto reason() const noexcept
            -> protocol::ofp_packet_in_reason
        {
            return protocol::ofp_packet_in_reason(packet_in_.reason);
        }

        auto frame() const noexcept
            -> boost::iterator_range<unsigned char const*>
        {
            return boost::make_iterator_range_n(data_.get(), frame_length());
        }

        auto frame_length() const noexcept
            -> std::uint16_t
        {
            return length() - base_pkt_in_size;
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
            detail::encode(container, packet_in_, base_pkt_in_size);
            return detail::encode_byte_array(
                    container, data_.get(), frame_length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_in
        {
            auto const pkt_in = detail::decode<v10_detail::ofp_packet_in>(
                    first, last, base_pkt_in_size);
            last = std::next(first, pkt_in.header.length - base_pkt_in_size);

            auto data = data_type{
                new unsigned char[std::distance(first, last)]
            };
            std::copy(first, last, data.get());
            first = last;

            return packet_in{pkt_in, std::move(data)};
        }

        static void validate(v10_detail::ofp_header const& header)
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
        packet_in(v10_detail::ofp_packet_in const& pkt_in, data_type&& data)
            : packet_in_(pkt_in)
            , data_(std::move(data))
        {
        }

    private:
        v10_detail::ofp_packet_in packet_in_;
        data_type data_;
    };

} // namespace message
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP
