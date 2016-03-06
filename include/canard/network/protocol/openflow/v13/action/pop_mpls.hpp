#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_MPLS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_MPLS_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class pop_mpls
        : public detail::v13::basic_action<
            pop_mpls, v13_detail::ofp_action_pop_mpls
          >
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_POP_MPLS;

        explicit pop_mpls(std::uint16_t const ethertype) noexcept
            : action_pop_mpls_{
                  action_type
                , length()
                , ethertype
                , { 0, 0 }
              }
        {
        }

        auto ethertype() const noexcept
            -> std::uint16_t
        {
            return action_pop_mpls_.ethertype;
        }

        static auto ipv4() noexcept
            -> pop_mpls
        {
            return pop_mpls{0x0800};
        }

        static auto unicast() noexcept
            -> pop_mpls
        {
            return pop_mpls{0x8847};
        }

        static auto multicast() noexcept
            -> pop_mpls
        {
            return pop_mpls{0x8848};
        }

    private:
        friend basic_action;

        explicit pop_mpls(raw_ofp_type const& action_pop_mpls) noexcept
            : action_pop_mpls_(action_pop_mpls)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return action_pop_mpls_;
        }

        static void validate_impl(pop_mpls const&)
        {
        }

    private:
        raw_ofp_type action_pop_mpls_;
    };

    inline auto operator==(pop_mpls const& lhs, pop_mpls const& rhs) noexcept
        -> bool
    {
        return lhs.ethertype() == rhs.ethertype();
    }

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_POP_MPLS_HPP
