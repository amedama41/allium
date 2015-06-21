#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class barrier_request
        : public v10_detail::basic_openflow_message<barrier_request>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_BARRIER_REQUEST;

        explicit barrier_request(std::uint32_t const xid = get_xid())
            : header_{protocol::OFP_VERSION, message_type, sizeof(header_), xid}
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return header_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> barrier_request
        {
            auto const header = detail::decode<v10_detail::ofp_header>(first, last);
            return barrier_request{header};
        }

    private:
        explicit barrier_request(v10_detail::ofp_header const header)
            : header_(header)
        {
            if (version() != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
        }

    private:
        v10_detail::ofp_header header_;
    };


    class barrier_reply
        : public v10_detail::basic_openflow_message<barrier_reply>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_BARRIER_REPLY;

        explicit barrier_reply(barrier_request const& request)
            : header_{protocol::OFP_VERSION, message_type, sizeof(header_), request.xid()}
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return header_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> barrier_reply
        {
            auto const header = detail::decode<v10_detail::ofp_header>(first, last);
            return barrier_reply{header};
        }

    private:
        explicit barrier_reply(v10_detail::ofp_header const header)
            : header_(header)
        {
            if (version() != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
        }

    private:
        v10_detail::ofp_header header_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP
