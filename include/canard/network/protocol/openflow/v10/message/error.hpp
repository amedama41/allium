#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_HPP

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class error
        : public v10_detail::basic_openflow_message<error>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_ERROR;

        auto header() const
            -> v10_detail::ofp_header
        {
            return error_.header;
        }

        auto error_type() const
            -> protocol::ofp_error_type
        {
            return protocol::ofp_error_type(error_.type);
        }

        auto error_code() const
            -> std::uint16_t
        {
            return error_.code;
        }

        auto data() const
            -> std::vector<std::uint8_t> const&
        {
            return data_;
        }

        auto failed_request_header() const
            -> v10_detail::ofp_header
        {
            auto it = data_.begin();
            return detail::decode<v10_detail::ofp_header>(it, data_.end());
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, error_);
            return container.push_back(data_.data(), data_.size());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> error
        {
            auto const error_msg
                = detail::decode<v10_detail::ofp_error_msg>(first, last);
            auto data = std::vector<std::uint8_t>(first, last);
            return error{error_msg, std::move(data)};
        }

    private:
        error(v10_detail::ofp_error_msg const& error, std::vector<std::uint8_t> data)
            : error_(error)
            , data_(std::move(data))
        {
            if (version() != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (error_type() != protocol::OFPET_HELLO_FAILED
                    && data_.size() < sizeof(v10_detail::ofp_header)) {
                throw std::runtime_error{"too short failed request size"};
            }
        }

    private:
        v10_detail::ofp_error_msg error_;
        std::vector<std::uint8_t> data_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_HPP
