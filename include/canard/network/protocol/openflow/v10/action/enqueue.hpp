#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v10/detail/action_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    class enqueue
        : public v10_detail::action_adaptor<enqueue, v10_detail::ofp_action_enqueue>
    {
        using ofp_action_t = v10_detail::ofp_action_enqueue;

    public:
        static protocol::ofp_action_type const action_type
            = protocol::OFPAT_ENQUEUE;

        enqueue(std::uint16_t const port, std::uint32_t const queue_id)
            : enqueue_{action_type, sizeof(ofp_action_t), port, {0}, queue_id}
        {
            validate_port(enqueue_.port);
        }

        auto port() const
            -> std::uint16_t
        {
            return enqueue_.port;
        }

        auto queue_id() const
            -> std::uint32_t
        {
            return enqueue_.queue_id;
        }

    private:
        friend action_adaptor;

        auto ofp_action() const
            -> ofp_action_t const&
        {
            return enqueue_;
        }

        explicit enqueue(ofp_action_t const& action_enqueue)
            : enqueue_(action_enqueue)
        {
            validate_port(enqueue_.port);
        }

        static void validate_port(std::uint16_t const port)
        {
            if (port > protocol::OFPP_MAX && port != protocol::OFPP_IN_PORT) {
                throw std::runtime_error{"invalid port"};
            }
        }

    private:
        ofp_action_t enqueue_;
    };

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_ENQUEUE_HPP
