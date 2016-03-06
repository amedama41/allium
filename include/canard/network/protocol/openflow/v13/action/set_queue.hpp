#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_QUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_QUEUE_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v13/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class set_queue
        : public detail::v13::basic_action<
            set_queue, v13_detail::ofp_action_set_queue
          >
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_SET_QUEUE;

        explicit set_queue(std::uint32_t const queue_id) noexcept
            : action_set_queue_{
                  action_type
                , length()
                , queue_id
              }
        {
        }

        auto queue_id() const noexcept
            -> std::uint32_t
        {
            return action_set_queue_.queue_id;
        }

    private:
        friend basic_action;

        explicit set_queue(raw_ofp_type const& action_set_queue) noexcept
            : action_set_queue_(action_set_queue)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return action_set_queue_;
        }

        static void validate_impl(set_queue const& action)
        {
            if (action.queue_id() == protocol::OFPQ_ALL) {
                throw std::runtime_error{"invalid queue_id"};
            }
        }

    private:
        raw_ofp_type action_set_queue_;
    };

    inline auto operator==(set_queue const& lhs, set_queue const& rhs) noexcept
        -> bool
    {
        return lhs.queue_id() == rhs.queue_id();
    }

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_QUEUE_HPP
