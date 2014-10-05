#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_SWITCH_INFO_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_SWITCH_INFO_HPP

#include <cstdint>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/optional/optional.hpp>
#include <canard/mac_address.hpp>
#include <canard/nabe/lldp.hpp>
#include <canard/network/protocol/openflow/v13/action/output.hpp>
#include <canard/network/protocol/openflow/v13/controller.hpp>
#include <canard/network/protocol/openflow/v13/message/packet_out.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/port.hpp>
#include "port_id.hpp"
#include "port_state.hpp"

namespace topology {

    namespace detail {

        auto to_hex_string(std::uint64_t const dpid)
            -> std::string
        {
            return (std::ostringstream{} << "0x" << std::hex << dpid).str();
        }

        auto create_lldp(std::uint64_t const dpid, canard::network::openflow::v13::port const& port)
            -> std::vector<std::uint8_t>
        {
            namespace nabe = canard::nabe;
            std::vector<std::uint8_t> buffer;
            nabe::encode(nabe::ether_header{
                      canard::mac_address{{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}}}
                    , port.hardware_address()
            } << nabe::lldp(
                      nabe::lldptlv::chassis_id{7, detail::to_hex_string(dpid)}
                    , nabe::lldptlv::port_id{7, std::to_string(port.port_no())}
                    , nabe::lldptlv::time_to_live{180}
            ), buffer);
            return buffer;
        }

    } // namespace detail

    template <class Topology>
    class switch_info
    {
        using of_port = canard::network::openflow::v13::port;
        using channel_ptr = typename canard::network::openflow::v13::controller<Topology>::channel_ptr;

    public:
        explicit switch_info(Topology& topology, channel_ptr channel)
            : channel_(std::move(channel))
            , port_state_map_{}
            , topology_(&topology)
        {
        }

        auto emplace(of_port port)
            -> boost::optional<topology::port_state<switch_info>&>
        {
            auto const emplace_result = port_state_map_.emplace(
                      std::piecewise_construct
                    , std::forward_as_tuple(port.port_no())
                    , std::forward_as_tuple(*this, std::move(port), topology_->get_io_service()));
            if (!emplace_result.second) {
                return boost::none;
            }
            return emplace_result.first->second;
        }

        void erase(of_port const& port)
        {
            auto const it = port_state_map_.find(port.port_no());
            if (it == port_state_map_.end()) {
                return;
            }
            port_state_map_.erase(it);
        }

        void update(of_port const& port)
        {
            if (auto const port_state = this->port_state(port.port_no())) {
                port_state->update(port);
            }
        }

        void discover_link(std::uint32_t const port_no, port_id const& opposite_port_id)
        {
            if (auto const port_state = this->port_state(port_no)) {
                port_state->discover_link(opposite_port_id);
            }
            else {
                BOOST_LOG_TRIVIAL(debug) << boost::format{"discover link from unkown port(0x%x,%u) to (%x,%u)"}
                    % get_datapath_id() % port_no % opposite_port_id.dpid() % opposite_port_id.port_no();
            }
        }

        void send_lldp_from(of_port const& port)
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"send LLDP from (0x%x,%u)"}
                % get_datapath_id() % port.port_no();

            namespace of13 = canard::network::openflow::v13;
            channel_->send(of13::packet_out{
                      detail::create_lldp(get_datapath_id(), port)
                    , of13::OFPP_CONTROLLER
                    , of13::actions::output{port.port_no()}
            });
        }

        void handle_link_up(of_port const& port, port_id const& opposite_port_id)
        {
            topology_->handle(up_link_info{{get_datapath_id(), port.port_no()}, opposite_port_id});
        }

        void handle_link_down(of_port const& port, port_id const& opposite_port_id)
        {
            topology_->handle(down_link_info{{get_datapath_id(), port.port_no()}, opposite_port_id});
        }

    private:
        auto get_datapath_id() const
            -> std::uint64_t
        {
            return topology_->get_datapath_id(channel_);
        }

        auto port_state(std::uint32_t const port_no)
            -> boost::optional<topology::port_state<switch_info>&>
        {
            auto const it = port_state_map_.find(port_no);
            if (it == port_state_map_.end()) {
                return boost::none;
            }
            return it->second;
        }

    private:
        channel_ptr channel_;
        std::unordered_map<std::uint32_t, topology::port_state<switch_info>> port_state_map_;
        Topology* topology_;
    };

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_SWITCH_INFO_HPP
