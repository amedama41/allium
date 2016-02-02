#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_ADAPTOR_HPP

#include <cstdint>
#include <boost/utility/string_ref.hpp>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace v13_detail {

    template <class T>
    class port_adaptor
    {
    public:
        auto port_no() const noexcept
            -> std::uint32_t
        {
            return base_port().port_no;
        }

        auto hardware_address() const noexcept
            -> canard::mac_address
        {
            return canard::mac_address{base_port().hw_addr};
        }

        auto name() const
            -> boost::string_ref
        {
            return base_port().name;
        }

        auto config() const noexcept
            -> std::uint32_t
        {
            return base_port().config;
        }

        auto state() const noexcept
            -> std::uint32_t
        {
            return base_port().state;
        }

        auto current_features() const noexcept
            -> std::uint32_t
        {
            return base_port().curr;
        }

        auto advertised_features() const noexcept
            -> std::uint32_t
        {
            return base_port().advertised;
        }

        auto supported_features() const noexcept
            -> std::uint32_t
        {
            return base_port().supported;
        }

        auto peer_advertised_features() const noexcept
            -> std::uint32_t
        {
            return base_port().peer;
        }

        auto current_speed() const noexcept
            -> std::uint32_t
        {
            return base_port().curr_speed;
        }

        auto max_speed() const noexcept
            -> std::uint32_t
        {
            return base_port().max_speed;
        }

        auto is_reserved() const
            -> bool
        {
            return protocol::OFPP_MAX < port_no();
        }

        auto administratively_down() const noexcept
            -> bool
        {
            return config() & protocol::OFPPC_PORT_DOWN;
        }

        auto link_down() const noexcept
            -> bool
        {
            return state() & protocol::OFPPS_LINK_DOWN;
        }

    private:
        auto base_port() const noexcept
            -> v13_detail::ofp_port const&
        {
            return static_cast<T const*>(this)->ofp_port();
        }
    };

} // namespace v13_detail
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_ADAPTOR_HPP
