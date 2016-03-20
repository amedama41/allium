#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <canard/network/protocol/openflow/v10/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    class enqueue
        : public actions_detail::basic_action<
                enqueue, v10_detail::ofp_action_enqueue
          >
    {
        using raw_ofp_type = v10_detail::ofp_action_enqueue;

    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_ENQUEUE;

        enqueue(std::uint32_t const queue_id
              , std::uint16_t const port_no) noexcept
            : enqueue_{
                  action_type
                , sizeof(raw_ofp_type)
                , port_no
                , { 0, 0, 0, 0, 0, 0 }
                , queue_id
              }
        {
        }

        auto queue_id() const noexcept
            -> std::uint32_t
        {
            return enqueue_.queue_id;
        }

        auto port_no() const noexcept
            -> std::uint16_t
        {
            return enqueue_.port;
        }

        template <class Action>
        static auto validate(Action&& action)
            -> typename std::enable_if<
                  is_same_value_type<Action, enqueue>::value, Action&&
               >::type
        {
            if (action.queue_id() == protocol::OFPQ_ALL) {
                throw std::runtime_error{"invalid queue_id"};
            }
            auto const port_no = action.port_no();
            if (port_no == 0
                    || (port_no > protocol::OFPP_MAX
                        && port_no != protocol::OFPP_IN_PORT)) {
                throw std::runtime_error{"invalid port_no"};
            }
            return std::forward<Action>(action);
        }

    private:
        friend basic_action;

        explicit enqueue(raw_ofp_type const& action_enqueue) noexcept
            : enqueue_(action_enqueue)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return enqueue_;
        }

    private:
        raw_ofp_type enqueue_;
    };

    inline auto operator==(enqueue const& lhs, enqueue const& rhs) noexcept
        -> bool
    {
        return lhs.queue_id() == rhs.queue_id()
            && lhs.port_no() == rhs.port_no();
    }

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP
