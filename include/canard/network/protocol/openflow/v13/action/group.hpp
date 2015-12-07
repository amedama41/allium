#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_GROUP_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_GROUP_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class group
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_GROUP;

            explicit group(std::uint32_t const group_id)
                : group_{action_type, sizeof(v13_detail::ofp_action_group), group_id}
            {
                if (group_id > protocol::OFPG_MAX) {
                    throw 3;
                }
            };

            auto type() const
                -> protocol::ofp_action_type
            {
                return action_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_action_group);
            }

            auto group_id() const
                -> std::uint32_t
            {
                return group_.group_id;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return v13_detail::encode(container, group_);
            }

        private:
            explicit group(v13_detail::ofp_action_group const& action_group)
                : group_(action_group)
            {
                if (group_.type != action_type) {
                    throw 1;
                }
                if (group_.len != sizeof(v13_detail::ofp_action_group)) {
                    throw 2;
                }
                if (group_id() > protocol::OFPG_MAX) {
                    throw 3;
                }
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> group
            {
                auto const action_group = v13_detail::decode<v13_detail::ofp_action_group>(first, last);
                return group{action_group};
            }

        private:
            v13_detail::ofp_action_group group_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_GROUP_HPP
