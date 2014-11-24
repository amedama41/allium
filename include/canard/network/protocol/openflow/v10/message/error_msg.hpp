#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_MSG_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_MSG_HPP

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class error_msg
        : public detail::basic_openflow_message<error_msg>
    {
    public:
        static ofp_type const message_type = OFPT_ERROR;

        auto header() const
            -> detail::ofp_header
        {
            return error_.header;
        }

        auto error_type() const
            -> ofp_error_type
        {
            return ofp_error_type(error_.type);
        }

        auto error_code() const
            -> std::uint16_t
        {
            return error_.code;
        }

        auto failed_request_header() const
            -> detail::ofp_header
        {
            auto it = data_.begin();
            return openflow::detail::decode<detail::ofp_header>(it, data_.end());
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            openflow::detail::encode(container, error_);
            return boost::push_back(container, data_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> error_msg
        {
            auto const error = openflow::detail::decode<detail::ofp_error_msg>(first, last);
            auto data = std::vector<std::uint8_t>(first, last);
            return error_msg{error, std::move(data)};
        }

    private:
        error_msg(detail::ofp_error_msg const& error, std::vector<std::uint8_t> data)
            : error_(error)
            , data_(std::move(data))
        {
            if (version() != OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (data_.size() < sizeof(detail::ofp_header)) {
                throw std::runtime_error{"too short failed request size"};
            }
        }

    private:
        detail::ofp_error_msg error_;
        std::vector<std::uint8_t> data_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_ERROR_MSG_HPP