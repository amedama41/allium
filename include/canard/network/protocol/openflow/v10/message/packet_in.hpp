#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PACKET_IN_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <boost/endian/conversion.hpp>
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

    namespace packet_in_detail {

        auto const pkt_in_size
            = offsetof(v10_detail::ofp_packet_in, pad)
            + sizeof(v10_detail::ofp_packet_in::pad);

        auto packet_in_byte_range(v10_detail::ofp_packet_in& pkt_in)
            -> boost::iterator_range<unsigned char*>
        {
            return boost::make_iterator_range(
                      reinterpret_cast<unsigned char*>(&pkt_in)
                    , reinterpret_cast<unsigned char*>(&pkt_in) + pkt_in_size);
        }

        template <class Iterator>
        auto decode_packet_in(Iterator& first, Iterator last)
            -> v10_detail::ofp_packet_in
        {
            auto pkt_in = v10_detail::ofp_packet_in{};
            auto const pkt_in_last = std::next(first, pkt_in_size);
            boost::overwrite(
                      boost::make_iterator_range(first, pkt_in_last)
                    , packet_in_byte_range(pkt_in));
            first = pkt_in_last;
            boost::endian::big_to_native_inplace(pkt_in);
            return pkt_in;
        }

    } // namespace packet_in_detail

    class packet_in
        : public v10_detail::basic_openflow_message<packet_in>
    {
        struct array_deleter
        {
            void operator()(unsigned char* ptr) const
            {
                delete [] ptr;
            }
        };

        using data_type = std::unique_ptr<unsigned char[], array_deleter>;

    public:
        static protocol::ofp_type const message_type = protocol::OFPT_PACKET_IN;

        packet_in(packet_in const& other)
            : packet_in_(other.packet_in_)
            , data_{new unsigned char[other.frame_length()]}
        {
            std::copy(other.data_.get()
                    , other.data_.get() + other.frame_length()
                    , data_.get());
        }

        packet_in(packet_in&&) = default;

        auto operator=(packet_in const& other)
            -> packet_in&
        {
            auto pkt_in = other;
            packet_in_ = pkt_in.packet_in_;
            data_.swap(pkt_in.data_);
            return *this;
        }

        auto operator=(packet_in&&)
            -> packet_in& = default;

        auto header() const
            -> v10_detail::ofp_header
        {
            return packet_in_.header;
        }

        auto buffer_id() const
            -> std::uint32_t
        {
            return packet_in_.buffer_id;
        }

        auto total_length() const
            -> std::uint16_t
        {
            return packet_in_.total_len;
        }

        auto in_port() const
            -> std::uint16_t
        {
            return packet_in_.in_port;
        }

        auto reason() const
            -> protocol::ofp_packet_in_reason
        {
            return protocol::ofp_packet_in_reason(packet_in_.reason);
        }

        auto frame() const
            -> boost::iterator_range<unsigned char*>
        {
            return boost::make_iterator_range(
                    data_.get(), data_.get() + frame_length());
        }

        auto frame_length() const
            -> std::uint16_t
        {
            return length() - packet_in_detail::pkt_in_size;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto pkt_in = packet_in_;
            boost::endian::native_to_big_inplace(pkt_in);
            container.push_back(pkt_in, packet_in_detail::pkt_in_size);
            return container.push_back(data_.get(), frame_length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> packet_in
        {
            auto const pkt_in = packet_in_detail::decode_packet_in(first, last);
            auto data
                = data_type{new unsigned char[std::distance(first, last)]};
            std::copy(first, last, data.get());
            return packet_in{pkt_in, std::move(data)};
        }

    private:
        packet_in(v10_detail::ofp_packet_in const& pkt_in, data_type data)
            : packet_in_(pkt_in)
            , data_(std::move(data))
        {
            if (version() != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
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
