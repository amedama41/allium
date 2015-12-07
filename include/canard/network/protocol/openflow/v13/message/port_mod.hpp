#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_MOD_HPP

#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class port_mod
        : public v13_detail::basic_openflow_message<port_mod>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_PORT_MOD;

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return port_mod_.header;
        }

        auto port_no() const
            -> std::uint32_t
        {
            return port_mod_.port_no;
        }

        auto hardware_address() const
            -> canard::mac_address
        {
            return {port_mod_.hw_addr};
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return v13_detail::encode(container, port_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_mod
        {
            auto const mod = v13_detail::decode<v13_detail::ofp_port_mod>(first, last);
            if (port_mod.header.length != sizeof(v13_detail::ofp_port_mod)) {
                throw 2;
            }
            return port_mod{mod};
        }

    private:
        explicit port_mod(v13_detail::ofp_port_mod const& port_mod)
            : port_mod_(port_mod)
        {
        }

    private:
        v13_detail::ofp_port_mod port_mod_;
    };

} // namespace messages

using messages::port_mod;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_MOD_HPP
