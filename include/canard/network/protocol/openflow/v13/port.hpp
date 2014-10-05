#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_HPP

#include <cstdint>
#include <bitset>
#include <memory>
#include <string>
#include <utility>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class port
    {
    public:
        port(port const& other)
            : port_{new detail::ofp_port(*other.port_)}
        {
        }

        port(port&&) noexcept = default;

        auto operator=(port const& other)
            -> port&
        {
            port tmp{other};
            this->port_.swap(tmp.port_);
            return *this;
        }

        auto operator=(port&&) noexcept -> port& = default;

        auto port_no() const
            -> std::uint32_t
        {
            return port_->port_no;
        }

        auto is_reserved() const
            -> bool
        {
            return OFPP_MAX < port_no();
        }

        auto is_in_port() const
            -> bool
        {
            return port_no() == OFPP_IN_PORT;
        }

        auto is_table() const
            -> bool
        {
            return port_no() == OFPP_TABLE;
        }

        auto is_normal() const
            -> bool
        {
            return port_no() == OFPP_NORMAL;
        }

        auto hardware_address() const
            -> canard::mac_address
        {
            return canard::mac_address{port_->hw_addr};
        }

        auto name() const
            -> std::string
        {
            return port_->name;
        }

        auto config() const
            -> std::bitset<std::numeric_limits<std::uint32_t>::digits>
        {
            return {port_->config};
        }

        auto administratively_down() const
            -> bool
        {
            return port_->config & OFPPC_PORT_DOWN;
        }

        auto state() const
            -> std::bitset<std::numeric_limits<std::uint32_t>::digits>
        {
            return {port_->state};
        }

        auto link_down() const
            -> bool
        {
            return port_->state & OFPPS_LINK_DOWN;
        }

        auto curr_speed() const
            -> std::uint32_t
        {
            return port_->curr_speed;
        }

        auto max_speed() const
            -> std::uint32_t
        {
            return port_->max_speed;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, *port_);
        }

    private:
        explicit port(std::unique_ptr<detail::ofp_port> port)
            : port_(std::move(port))
        {
        }

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port
        {
            auto port_ptr = std::unique_ptr<detail::ofp_port>{new detail::ofp_port};
            *port_ptr = detail::decode<detail::ofp_port>(first, last);
            return port{std::move(port_ptr)};
        }

    private:
        std::unique_ptr<detail::ofp_port> port_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_HPP
