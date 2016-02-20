#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ERROR_HPP

#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/range/iterator_range.hpp>
#include <canard/network/protocol/openflow/binary_data.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class error
        : public v13_detail::basic_openflow_message<error>
    {
        static constexpr std::uint16_t base_err_msg_size
            = sizeof(v13_detail::ofp_error_msg);

    public:
        using data_type = binary_data::pointer_type;

        static constexpr protocol::ofp_type message_type = protocol::OFPT_ERROR;

        error(protocol::ofp_error_type const type
            , std::uint16_t const code
            , binary_data data
            , std::uint32_t const xid = get_xid()) noexcept
            : error_msg_{
                  v13_detail::ofp_header{
                        protocol::OFP_VERSION
                      , message_type
                      , std::uint16_t(base_err_msg_size + data.size())
                      , xid
                  }
                , std::uint16_t(type)
                , code
              }
            , data_(std::move(data).data())
        {
        }

        template <class Message>
        error(protocol::ofp_error_type const type
            , std::uint16_t const code
            , Message const& msg)
            : error{type, code, create_data(msg), msg.xid()}
        {
        }

        error(protocol::ofp_error_type const type
            , std::uint16_t const code
            , v13_detail::ofp_header const& header)
            : error{type, code, create_data(header), header.xid}
        {
        }

        error(error const& other)
            : error_msg_(other.error_msg_)
            , data_(binary_data::copy_data(other.data()))
        {
        }

        error(error&& other) noexcept
            : error_msg_(other.error_msg_)
            , data_(std::move(other.data_))
        {
            other.error_msg_.header.length = base_err_msg_size;
        }

        auto operator=(error const& other)
            -> error&
        {
            auto tmp = other;
            return operator=(std::move(tmp));
        }

        auto operator=(error&& other) noexcept
            -> error&
        {
            auto tmp = std::move(other);
            error_msg_ = tmp.error_msg_;
            data_.swap(tmp.data_);
            return *this;
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
        {
            return error_msg_.header;
        }

        auto error_type() const noexcept
            -> protocol::ofp_error_type
        {
            return protocol::ofp_error_type(error_msg_.type);
        }

        auto error_code() const noexcept
            -> std::uint16_t
        {
            return error_msg_.code;
        }

        auto data() const noexcept
            -> boost::iterator_range<unsigned char const*>
        {
            return boost::make_iterator_range_n(data_.get(), data_length());
        }

        auto data_length() const noexcept
            -> std::uint16_t
        {
            return length() - base_err_msg_size;
        }

        auto extract_data() noexcept
            -> binary_data
        {
            auto const data_len = data_length();
            error_msg_.header.length = base_err_msg_size;
            return binary_data{std::move(data_), data_len};
        }

        auto failed_request_header() const
            -> v13_detail::ofp_header
        {
            auto it = data_.get();
            auto const it_end = data_.get() + data_length();
            return detail::decode<v13_detail::ofp_header>(it, it_end);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, error_msg_);
            return detail::encode_byte_array(
                    container, data_.get(), data_length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> error
        {
            auto const error_msg
                = detail::decode<v13_detail::ofp_error_msg>(first, last);

            auto const data_length
                = error_msg.header.length - sizeof(v13_detail::ofp_error_msg);
            last = std::next(first, data_length);
            auto data = binary_data::copy_data(first, last);
            first = last;

            return error{error_msg, std::move(data)};
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < base_err_msg_size) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        error(v13_detail::ofp_error_msg const& error_msg
            , data_type&& data) noexcept
            : error_msg_(error_msg)
            , data_(std::move(data))
        {
        }

        template <class Message>
        static auto create_data(Message const& msg)
            -> binary_data
        {
            auto buffer = std::vector<unsigned char>{};
            buffer.reserve(msg.length());
            msg.encode(buffer);

            constexpr auto max_data_size
                = std::numeric_limits<std::uint16_t>::max() - base_err_msg_size;
            if (buffer.size() > max_data_size) {
                buffer.resize(max_data_size);
            }

            return binary_data{buffer};
        }

        static auto create_data(v13_detail::ofp_header const& header)
            -> binary_data
        {
            auto buffer = std::vector<unsigned char>{};
            buffer.reserve(sizeof(v13_detail::ofp_header));
            detail::encode(buffer, header);

            return binary_data{buffer};
        }

    private:
        v13_detail::ofp_error_msg error_msg_;
        data_type data_;
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ERROR_HPP
