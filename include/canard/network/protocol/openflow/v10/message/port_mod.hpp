#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_MOD_HPP

#include <cstdint>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/port.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class port_mod
        : public v10_detail::basic_openflow_message<port_mod>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_PORT_MOD;

        port_mod(std::uint16_t const port_no, canard::mac_address const& macaddr
                , std::uint32_t const config, std::uint32_t const mask
                , std::uint32_t const advertise
                , std::uint32_t const xid = get_xid())
            : port_mod_{
                  {protocol::OFP_VERSION, message_type, sizeof(port_mod_), xid}
                , port_no
                , {
                      macaddr.to_bytes()[0], macaddr.to_bytes()[1]
                    , macaddr.to_bytes()[2], macaddr.to_bytes()[3]
                    , macaddr.to_bytes()[4], macaddr.to_bytes()[5]
                  }
                , config, mask, advertise, {0}
              }
        {
        }

        port_mod(port const& port
                , std::uint32_t const config, std::uint32_t const mask
                , std::uint32_t const advertise
                , std::uint32_t const xid = get_xid())
            : port_mod{
                  port.port_no(), port.hardware_address()
                , config, mask, advertise, xid
              }
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return port_mod_.header;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, port_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_mod
        {
            return port_mod{
                detail::decode<v10_detail::ofp_port_mod>(first, last)
            };
        }

    private:
        explicit port_mod(v10_detail::ofp_port_mod const& port_mod)
            : port_mod_(port_mod)
        {
        }

    private:
        v10_detail::ofp_port_mod port_mod_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_PORT_MOD_HPP
