#ifndef CANARD_NETWORK_OPENFLOW_V10_PORT_HPP
#define CANARD_NETWORK_OPENFLOW_V10_PORT_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class port
    {
    public:
        auto port_no() const
            -> std::uint16_t
        {
            return port_.port_no;
        }

        template <class Container>
        auto encode(Container& container)
            -> Container&
        {
            return detail::encode(container, port_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port
        {
            auto const phy_port = detail::decode<v10_detail::ofp_phy_port>(first, last);
            return port{phy_port};
        }

    private:
        explicit port(v10_detail::ofp_phy_port const& phy_port)
            : port_(phy_port)
        {
        }

    private:
        v10_detail::ofp_phy_port port_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_PORT_HPP
