#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_STATUS_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_STATUS_HPP

#include <memory>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/port.hpp>
#include "port_id.hpp"
#include "link_state.hpp"

namespace topology {

    template <class SwitchInfo>
    class port_state
    {
        using of_port = canard::network::openflow::v13::port;
        using link_state = topology::link_state<port_state>;
        using link_up = topology::link_up<port_state>;

    public:
        port_state(SwitchInfo& sw_info, of_port port, boost::asio::io_service& io_service)
            : port_(std::move(port))
            , link_state_{boost::msm::back::states_ << link_up{this}}
            , timer_{new boost::asio::steady_timer{io_service}}
            , sw_info_(&sw_info)
        {
            link_state_.start();
            if (!port_.administratively_down() && !port_.link_down()) {
                up_link();
            }
        }

        port_state(port_state const&) = delete;

        port_state(port_state&& other)
            : port_(std::move(other))
            , link_state_{boost::msm::back::states_ << link_up{this}}
            , timer_{std::move(other.timer_)}
            , sw_info_(other.sw_info_)
        {
            timer_->cancel();
            other.link_state_.stop();
            link_state_.start();
            if (!port_.administratively_down() && !port_.link_down()) {
                up_link();
            }
        }

        ~port_state()
        {
            timer_->cancel();
            link_state_.stop();
        }

        void update(of_port const& port)
        {
            port_ = port;
            if (!port_.administratively_down() && !port_.link_down()) {
                up_link();
            }
            else {
                down_link();
            }
        }

        template <class Duration1, class Duration2>
        void send_lldp(Duration1&& send_time, Duration2&& timeout)
        {
            timer_->expires_from_now(std::forward<Duration1>(send_time));
            timer_->async_wait([=](boost::system::error_code const& ec) {
                if (ec) {
                    return;
                }
                timer_->expires_from_now(timeout);
                timer_->async_wait([this](boost::system::error_code const& ec) {
                    if (ec) {
                        return;
                    }
                    this->timeout();
                });
                sw_info_->send_lldp_from(port_);
            });
        }

        void up_link()
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"up_link event occure on %u"}
                % port_.port_no();
            link_state_.process_event(topology::up_link{});
        }

        void down_link()
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"down_link event occure on %u"}
                % port_.port_no();
            timer_->cancel();
            link_state_.process_event(topology::down_link{});
        }

        void discover_link(port_id const& port_id)
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"discover_link event occure on %u"}
                % port_.port_no();
            timer_->cancel();
            link_state_.process_event(topology::discover_link{port_id});
        }

        void timeout()
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"timeout event occure on %u"}
                % port_.port_no();
            link_state_.process_event(topology::timeout{});
        }

        void handle_link_up(port_id const& opposite_port_id)
        {
            sw_info_->handle_link_up(port_, opposite_port_id);
        }

        void handle_link_down(port_id const& opposite_port_id)
        {
            sw_info_->handle_link_down(port_, opposite_port_id);
        }

    private:
        of_port port_;
        link_state link_state_;
        std::unique_ptr<boost::asio::steady_timer> timer_;
        SwitchInfo* sw_info_;
    };

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_PORT_STATUS_HPP
