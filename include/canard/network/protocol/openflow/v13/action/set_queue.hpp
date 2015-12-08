#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_QUEUE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_QUEUE_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class set_queue
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_SET_QUEUE;

            explicit set_queue(std::uint32_t const queue_id)
                : set_queue_{action_type, sizeof(v13_detail::ofp_action_set_queue), queue_id}
            {
                if (queue_id == protocol::OFPQ_ALL) {
                    throw 3;
                }
            }

            auto type() const
                -> protocol::ofp_action_type
            {
                return action_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_action_set_queue);
            }

            auto queue_id() const
                -> std::uint32_t
            {
                return set_queue_.queue_id;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, set_queue_);
            }

        private:
            explicit set_queue(v13_detail::ofp_action_set_queue const& action_set_queue)
                : set_queue_(action_set_queue)
            {
                if (set_queue_.type != type()) {
                    throw 1;
                }
                if (set_queue_.len != length()) {
                    throw 2;
                }
                if (set_queue_.queue_id == protocol::OFPQ_ALL) {
                    throw 3;
                }
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> set_queue
            {
                auto const action_set_queue = detail::decode<v13_detail::ofp_action_set_queue>(first, last);
                return set_queue{action_set_queue};
            }

        private:
            v13_detail::ofp_action_set_queue set_queue_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_QUEUE_HPP
