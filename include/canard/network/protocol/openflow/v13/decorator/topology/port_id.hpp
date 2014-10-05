#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_ID_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_ID_HPP

#include <cstdint>
#include <utility>

namespace topology {

    class port_id
    {
    public:
        port_id() = default;

        port_id(std::uint64_t const dpid, std::uint32_t const port_no)
            : dpid_and_port_no_{dpid, port_no}
        {
        }

        auto dpid() const
            -> std::uint64_t
        {
            return dpid_and_port_no_.first;
        }

        auto port_no() const
            -> std::uint32_t
        {
            return dpid_and_port_no_.second;
        }

    private:
        std::pair<std::uint64_t, std::uint32_t> dpid_and_port_no_;
    };

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_ID_HPP
