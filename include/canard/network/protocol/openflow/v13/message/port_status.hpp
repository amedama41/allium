#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_STATUS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_STATUS_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/port.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class port_status
        : public v13_detail::basic_openflow_message<port_status>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_PORT_STATUS;

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return header_;
        }

        auto reason() const
            -> protocol::ofp_port_reason
        {
            return protocol::ofp_port_reason(reason_);
        }

        auto port() const
            -> v13::port const&
        {
            return port_;
        }

    private:
        port_status(v13_detail::ofp_header const& header, std::uint8_t reason, v13::port port)
            : header_(header)
            , reason_(reason)
            , port_{std::move(port)}
        {
        }

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_status
        {
            auto const header = detail::decode<v13_detail::ofp_header>(first, last);
            auto const reason = detail::decode<std::uint8_t>(first, last);

            std::advance(first, sizeof(decltype(v13_detail::ofp_port_status::pad)));

            auto port = v13::port::decode(first, last);

            return {header, reason, std::move(port)};
        }

    private:
        v13_detail::ofp_header header_;
        std::uint8_t reason_;
        v13::port port_;
    };

} // namespace messages

using messages::port_status;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_STATUS_HPP
