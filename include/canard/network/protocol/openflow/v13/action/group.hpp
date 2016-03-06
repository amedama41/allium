#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_GROUP_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_GROUP_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v13/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class group
        : public detail::v13::basic_action<group, v13_detail::ofp_action_group>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_GROUP;

        explicit group(std::uint32_t const group_id) noexcept
            : action_group_{
                  action_type
                , length()
                , group_id
              }
        {
        };

        auto group_id() const noexcept
            -> std::uint32_t
        {
            return action_group_.group_id;
        }

    private:
        friend basic_action;

        explicit group(
                v13_detail::ofp_action_group const& action_group) noexcept
            : action_group_(action_group)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return action_group_;
        }

        static void validate_impl(group const& action)
        {
            if (action.group_id() > protocol::OFPG_MAX) {
                throw std::runtime_error{"invalid group_id"};
            }
        }

    private:
        raw_ofp_type action_group_;
    };

    inline auto operator==(group const& lhs, group const& rhs) noexcept
        -> bool
    {
        return lhs.group_id() == rhs.group_id();
    }

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_GROUP_HPP
