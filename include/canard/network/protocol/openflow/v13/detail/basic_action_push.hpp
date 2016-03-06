#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_PUSH_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_PUSH_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

    template <class T>
    class basic_action_push
        : public basic_action<T, openflow::v13::v13_detail::ofp_action_push>
    {
        using base_type
            = basic_action<T, openflow::v13::v13_detail::ofp_action_push>;

    public:
        using raw_ofp_type = typename base_type::raw_ofp_type;

        auto ethertype() const noexcept
            -> std::uint16_t
        {
            return action_push_.ethertype;
        }

        friend auto operator==(T const& lhs, T const& rhs) noexcept
            -> bool
        {
            return lhs.ethertype() == rhs.ethertype();
        }

    protected:
        explicit basic_action_push(std::uint16_t const ethertype) noexcept
            : action_push_{
                  T::action_type
                , base_type::length()
                , ethertype
                , { 0, 0 }
              }
        {
        }

        explicit basic_action_push(raw_ofp_type const& action_push) noexcept
            : action_push_(action_push)
        {
        }

    private:
        friend base_type;

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return action_push_;
        }

    private:
        raw_ofp_type action_push_;
    };

} // namespace v13
} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_PUSH_HPP
