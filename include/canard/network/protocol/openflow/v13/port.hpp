#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_HPP

#include <cstddef>
#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/port_adaptor.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class port
        : public v13_detail::port_adaptor<port>
    {
    public:
        static constexpr std::size_t base_size = sizeof(v13_detail::ofp_port);

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(v13_detail::ofp_port);
        }

        auto ofp_port() const noexcept
            -> v13_detail::ofp_port const&
        {
            return port_;
        }

        auto is_in_port() const
            -> bool
        {
            return port_no() == protocol::OFPP_IN_PORT;
        }

        auto is_table() const
            -> bool
        {
            return port_no() == protocol::OFPP_TABLE;
        }

        auto is_normal() const
            -> bool
        {
            return port_no() == protocol::OFPP_NORMAL;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, port_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port
        {
            return port{detail::decode<v13_detail::ofp_port>(first, last)};
        }

        static auto from_ofp_port(v13_detail::ofp_port const& ofp_port) noexcept
            -> port
        {
            return port{ofp_port};
        }

    private:
        explicit port(v13_detail::ofp_port const& port) noexcept
            : port_(port)
        {
        }

    private:
        v13_detail::ofp_port port_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_HPP
