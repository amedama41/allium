#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATUS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATUS_HPP

#include <cstdint>
#include <array>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/port_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class port_status
        : public v10_detail::basic_openflow_message<port_status>
        , public v10_detail::port_adaptor<port_status>
    {
    public:
        static ofp_type const message_type = OFPT_PORT_STATUS;

        auto header() const
            -> v10_detail::ofp_header
        {
            return port_status_.header;
        }

        auto reason() const
            -> ofp_port_reason
        {
            return ofp_port_reason(port_status_.reason);
        }

        auto is_added() const
            -> bool
        {
            return reason() == OFPPR_ADD;
        }

        auto is_deleted() const
            -> bool
        {
            return reason() == OFPPR_DELETE;
        }

        auto is_modified() const
            -> bool
        {
            return reason() == OFPPR_MODIFY;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, port_status_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_status
        {
            auto const status = detail::decode<v10_detail::ofp_port_status>(first, last);
            return port_status{status};
        }

    private:
        explicit port_status(v10_detail::ofp_port_status const& status)
            : port_status_(status)
        {
            if (version() != v10::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (type() != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
        }

        friend port_adaptor;

        auto ofp_phy_port() const
            -> v10_detail::ofp_phy_port const&
        {
            return port_status_.desc;
        }

    private:
        v10_detail::ofp_port_status port_status_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATUS_HPP
