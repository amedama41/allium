#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP

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

    class strip_vlan
        : public actions_detail::basic_action<
                strip_vlan, v10_detail::ofp_action_header
          >
    {
        using raw_ofp_type = v10_detail::ofp_action_header;

    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_STRIP_VLAN;

        strip_vlan() noexcept
            : strip_vlan_{action_type, sizeof(raw_ofp_type), { 0, 0, 0, 0 }}
        {
        }

        template <class Action>
        static auto validate(Action&& action)
            -> typename std::enable_if<
                  std::is_same<canard::remove_cvref_t<Action>, strip_vlan>::value
                , Action&&
               >::type
        {
            return std::forward<Action>(action);
        }

    private:
        friend basic_action;

        explicit strip_vlan(raw_ofp_type const& action_header) noexcept
            : strip_vlan_(action_header)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return strip_vlan_;
        }

    private:
        raw_ofp_type strip_vlan_;
    };

    constexpr auto operator==(strip_vlan const&, strip_vlan const&) noexcept
        -> bool
    {
        return true;
    }

    using pop_vlan = strip_vlan;

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_STRIP_VLAN_HPP
