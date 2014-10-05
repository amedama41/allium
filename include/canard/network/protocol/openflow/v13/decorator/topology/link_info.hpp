#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_UP_LINK_INFO_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_UP_LINK_INFO_HPP

#include "port_id.hpp"

namespace topology {

    struct up_link_tag {};
    struct down_link_tag {};

    template <class T>
    class link_info
    {
    public:
        link_info(port_id const& from, port_id const& to)
            : from_(from)
            , to_(to)
        {
        }

        auto from() const
            -> port_id
        {
            return from_;
        };

        auto to() const
            -> port_id
        {
            return to_;
        }

    private:
        port_id from_;
        port_id to_;
    };

    using up_link_info = link_info<up_link_tag>;
    using down_link_info = link_info<down_link_tag>;

    auto operator<<(std::ostream& os, up_link_info const& link_info)
        -> std::ostream&
    {
        return os << boost::format{"link up: (0x%x,%u)->(0x%x,%u)"}
            % link_info.from().dpid() % link_info.from().port_no()
            % link_info.to().dpid() % link_info.to().port_no();
    }

    auto operator<<(std::ostream& os, down_link_info const& link_info)
        -> std::ostream&
    {
        return os << boost::format{"link down: (0x%x,%u)->(0x%x,%u)"}
            % link_info.from().dpid() % link_info.from().port_no()
            % link_info.to().dpid() % link_info.to().port_no();
    }

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_UP_LINK_INFO_HPP
