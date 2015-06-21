#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_TOPOLOGY_DECORATOR_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_TOPOLOGY_DECORATOR_HPP

#include <cstdint>
#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/optional/optional.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/controller_decorator.hpp>
#include <canard/network/protocol/openflow/v13/decorator/auto_packet_in_decorator.hpp>
#include <canard/network/protocol/openflow/v13/decorator/negotiation_decorator.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/transaction.hpp>
#include <canard/packet_parser.hpp>
#include "link_info.hpp"
#include "switch_info.hpp"

namespace topology {

    template <class Base = canard::network::openflow::v13::null_decorator>
    struct topology_decorator
    {
        template <class Derived>
        class type;
    };

    template <class Base>
    template <class Derived>
    class topology_decorator<Base>::type
        : public canard::network::openflow::v13::decoration<
               type<Derived>
             , auto_packet_in_decorator< negotiation_decorator< Base >>>
    {
    public:
        template <class... Args>
        explicit type(boost::asio::io_service& io_service, Args&&... args)
            : type::base_type(std::forward<Args>(args)...)
            , io_service_(io_service)
        {
        }

        auto get_io_service()
            -> boost::asio::io_service&
        {
            return io_service_;
        }

        template <class ChannelPtr>
        void handle(ChannelPtr channel)
        {
            auto const dpid = this->get_datapath_id(channel);
            auto const emplace_result = switch_info_map_.emplace(dpid, topology::switch_info<type>{*this, channel});
            assert(emplace_result.second);

            namespace of13 = canard::network::openflow::v13;
            channel->send_request(of13::port_description_request{}
                    , [=](boost::system::error_code const& ec, of13::transaction<of13::port_description_reply> txn)
            {
                if (ec) {
                    BOOST_LOG_TRIVIAL(info)
                        << boost::format{"sening port description request to 0x%x failed due to %s"}
                        % dpid % ec.message();
                    return;
                }

                namespace ph = std::placeholders;
                txn.expires_from_now(std::chrono::seconds{10});
                txn.async_wait(std::bind(&type::handle_port_description, this, dpid, ph::_1, ph::_2));
            });
            static_cast<Derived*>(this)->handle(channel);
        }

        template <class ChannelPtr>
        void handle(ChannelPtr channel, canard::network::openflow::v13::disconnected_info info)
        {
            switch_info_map_.erase(this->get_datapath_id(channel));
            static_cast<Derived*>(this)->handle(std::move(channel), std::move(info));
        }

        template <class ChannelPtr>
        void handle(ChannelPtr channel, canard::network::openflow::v13::packet_in pkt_in)
        {
            canard::packet{pkt_in.frame()}.lldpdu([&](canard::lldpdu const& lldpdu) {
                BOOST_LOG_TRIVIAL(debug) << boost::format{"receive LLDP from (%s,%s)"}
                    % lldpdu.chassis_id() % lldpdu.port_id();
                if (auto const sw_info = switch_info(std::stoull(lldpdu.chassis_id(), nullptr, 16))) {
                    sw_info->discover_link(std::stoul(lldpdu.port_id()), {this->get_datapath_id(channel), pkt_in.in_port()});
                }
                else {
                    BOOST_LOG_TRIVIAL(info) << boost::format{"discover link from unknown switch (%s,%s) to (0x%x,%u)"}
                        % lldpdu.chassis_id() % lldpdu.port_id() % this->get_datapath_id(channel) % pkt_in.in_port();
                }
            });
            static_cast<Derived*>(this)->handle(std::move(channel), std::move(pkt_in));
        }

        template <class ChannelPtr>
        void handle(ChannelPtr channel, canard::network::openflow::v13::port_status port_status)
        {
            auto const sw_info = switch_info(this->get_datapath_id(channel));
            switch (port_status.reason()) {
            case canard::network::openflow::v13::protocol::OFPPR_ADD:
                sw_info->emplace(port_status.port());
                break;

            case canard::network::openflow::v13::protocol::OFPPR_DELETE:
                sw_info->erase(port_status.port());
                break;

            case canard::network::openflow::v13::protocol::OFPPR_MODIFY:
                sw_info->update(port_status.port());
                break;
            }
            static_cast<Derived*>(this)->handle(std::move(channel), std::move(port_status));
        }

        template <class... Args>
        void handle(Args&&... args)
        {
            static_cast<Derived*>(this)->handle(std::forward<Args>(args)...);
        }

        void handle(up_link_info&& up_link)
        {
            BOOST_LOG_TRIVIAL(info) << up_link;
            static_cast<Derived*>(this)->handle(std::move(up_link));
        }

        void handle(down_link_info&& down_link)
        {
            BOOST_LOG_TRIVIAL(info) << down_link;
            static_cast<Derived*>(this)->handle(std::move(down_link));
        }

    private:
        void handle_port_description(std::uint64_t const dpid
                , boost::system::error_code const& ec
                , boost::optional<canard::network::openflow::v13::port_description_reply> reply)
        {
            if (ec) {
                BOOST_LOG_TRIVIAL(info)
                    << boost::format{"receiving port description reply from 0x%x failed due to %s"}
                    % dpid % ec.message();
                return;
            }

            assert(static_cast<bool>(reply));
            auto const sw_info = switch_info(dpid);
            for (auto& port_desc : *reply) {
                sw_info->emplace(std::move(port_desc));
            }
        }

        auto switch_info(std::uint64_t const dpid)
            -> boost::optional<topology::switch_info<type>&>
        {
            auto const it = switch_info_map_.find(dpid);
            if (it == switch_info_map_.end()) {
                return boost::none;
            }
            return it->second;
        }

    private:
        std::unordered_map<std::uint64_t, topology::switch_info<type>> switch_info_map_;
        boost::asio::io_service& io_service_;
    };

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_TOPOLOGY_DECORATOR_HPP
