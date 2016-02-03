#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATUS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_STATUS_HPP

#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/detail/port_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/port.hpp>

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
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_PORT_STATUS;

        port_status(v10::protocol::ofp_port_reason const reason
                  , v10::port const& port
                  , std::uint32_t const xid = get_xid()) noexcept
            : port_status_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , sizeof(v10_detail::ofp_port_status)
                    , xid
                  }
                , std::uint8_t(reason)
                , { 0, 0, 0, 0, 0, 0, 0 }
                , port.ofp_port()
              }
        {
        }

        auto header() const noexcept
            -> v10_detail::ofp_header const&
        {
            return port_status_.header;
        }

        auto reason() const noexcept
            -> protocol::ofp_port_reason
        {
            return protocol::ofp_port_reason(port_status_.reason);
        }

        auto port() const noexcept
            -> v10::port
        {
            return v10::port::from_ofp_port(port_status_.desc);
        }

        auto is_added() const noexcept
            -> bool
        {
            return reason() == protocol::OFPPR_ADD;
        }

        auto is_deleted() const noexcept
            -> bool
        {
            return reason() == protocol::OFPPR_DELETE;
        }

        auto is_modified() const noexcept
            -> bool
        {
            return reason() == protocol::OFPPR_MODIFY;
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
            return port_status{
                detail::decode<v10_detail::ofp_port_status>(first, last)
            };
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length != sizeof(v10_detail::ofp_port_status)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit port_status(v10_detail::ofp_port_status const& status) noexcept
            : port_status_(status)
        {
        }

        friend port_adaptor;

        auto ofp_port() const noexcept
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
