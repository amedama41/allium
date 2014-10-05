#ifndef CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_LINK_STATE_HPP
#define CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_LINK_STATE_HPP

#include <cstddef>
#include <chrono>
#include <utility>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include "port_id.hpp"

namespace topology {

    // Event
    struct timeout {};
    struct discover_link { port_id opposite_port_id; };
    struct up_link {};
    struct down_link {};

    // Actions
    struct resend_lldp
    {
        template <class Event, class FSM, class SourceState, class TargetState>
        void operator()(Event const&, FSM& fsm, SourceState& state, TargetState&)
        {
            ++state.retry_count;
            fsm.send_lldp(state.send_time());
        }
    };

    struct reset_count
    {
        template <class Event, class FSM, class SourceState, class TargetState>
        void operator()(Event const&, FSM&, SourceState& state, TargetState&)
        {
            state.retry_count = 0;
        }
    };

    // Guards
    struct failed_retry
    {
        template <class Event, class FSM, class SourceState, class TargetState>
        bool operator()(Event const&, FSM&, SourceState& src, TargetState&)
        {
            return src.retry_count == SourceState::max_retry_count;
        }
    };

    // State
    struct link_down : public boost::msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const& event, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "entering link_down";
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "exiting link_down";
        }
    };

    struct discovering : public boost::msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const& event, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "entering discovering";
            retry_count = 0;
            fsm.send_lldp(send_time());
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "exiting discovering";
        }

        auto send_time() const
            -> std::chrono::seconds
        {
            return std::chrono::seconds{2};
        }

        std::size_t retry_count;
        enum { max_retry_count = 5 };
    };

    struct stable : public boost::msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const& event, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"entering stable on %s"}
                % typeid(Event).name();
        }

        template <class FSM>
        void on_entry(discover_link const& event, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "entering stable";
            retry_count = 0;
            opposite_port_id = event.opposite_port_id;
            fsm.send_lldp(send_time());
            fsm.handle_link_up(opposite_port_id);
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "exiting stable";
            fsm.handle_link_down(opposite_port_id);
            opposite_port_id = port_id{};
        }

        auto send_time() const
            -> std::chrono::seconds
        {
            return std::chrono::seconds{10};
        }

        port_id opposite_port_id;
        std::size_t retry_count;
        enum { max_retry_count = 3 };
    };

    struct unstable : public boost::msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "entering unstable";
            retry_count = 0;
            fsm.send_lldp(send_time());
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "exiting unstable";
        }

        auto send_time() const
            -> std::chrono::seconds
        {
            return std::chrono::seconds{15};
        }

        std::size_t retry_count;
        enum { max_retry_count = 3 };
    };

    template <class PortState>
    class link_up_ : public boost::msm::front::state_machine_def<link_up_<PortState>>
    {
        template <class... Args>
        using row = boost::msm::front::Row<Args...>;
        using none = boost::msm::front::none;

    public:
        template <class Event, class FSM>
        void on_entry(Event const& event, FSM& fsm)
        {
            BOOST_LOG_TRIVIAL(debug) << "entering link_up";
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "exiting link_up";
        }

        using initial_state = discovering;

        struct transition_table : public boost::mpl::vector<
            //   Start          Event           Target         Action      Guard
            // +--------------+---------------+--------------+-------------+----------------+
            row< discovering  , timeout       , none         , resend_lldp , none           >,
            row< discovering  , timeout       , unstable     , none        , failed_retry   >,
            row< discovering  , discover_link , stable       , none        , none           >,
            // +--------------+---------------+--------------+-------------+----------------+
            row< stable       , timeout       , none         , resend_lldp , none           >,
            row< stable       , timeout       , unstable     , none        , failed_retry   >,
            row< stable       , discover_link , none         , reset_count , none           >,
            // +--------------+---------------+--------------+-------------+----------------+
            row< unstable     , timeout       , none         , resend_lldp , none           >,
            row< unstable     , discover_link , stable       , none        , none           >,
            row< unstable     , discover_link , unstable     , none        , failed_retry   >
            // +--------------+---------------+--------------+-------------+----------------+
        > {};

        link_up_() = default;

        link_up_(PortState* const port_state)
            : port_state_(port_state)
        {
        }

        template <class Event, class FSM>
        void no_transition(Event const&, FSM&, int state)
        {
            BOOST_LOG_TRIVIAL(debug) << boost::format{"no transition from state %u on event %s"}
                % state % typeid(Event).name();
        }

        template <class Duration>
        void send_lldp(Duration&& send_time)
        {
            port_state_->send_lldp(std::forward<Duration>(send_time), std::chrono::seconds{5});
        }

        void handle_link_up(port_id const& opposite_port_id)
        {
            port_state_->handle_link_up(opposite_port_id);
        }

        void handle_link_down(port_id const& opposite_port_id)
        {
            port_state_->handle_link_down(opposite_port_id);
        }

    private:
        PortState* port_state_;
    };

    template <class PortState>
    using link_up = boost::msm::back::state_machine<link_up_<PortState>>;

    template <class PortState>
    class link_state_ : public boost::msm::front::state_machine_def<link_state_<PortState>>
    {
        template <class... Args>
        using row = boost::msm::front::Row<Args...>;
        using none = boost::msm::front::none;

    public:
        using link_up = link_up<PortState>;

        using initial_state = link_down;

        struct transition_table : public boost::mpl::vector<
            row< link_down    , up_link       , link_up      , none      , none           >,
            row< link_up      , down_link     , link_down    , none      , none           >
        > {};

        template <class Event, class FSM>
        void no_transition(Event const&, FSM&, int)
        {
        }
    };

    template <class PortState>
    using link_state = boost::msm::back::state_machine<link_state_<PortState>>;

} // namespace topology

#endif // CANARD_NETWORK_OPENFLOW_DECOLATOR_TOPOLOGY_LINK_STATE_HPP
