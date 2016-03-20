#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_CLEAR_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_CLEAR_ACTIONS_HPP

#include <canard/network/protocol/openflow/v13/detail/basic_instruction.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace instructions {

    class clear_actions
        : public detail::v13::basic_instruction<
            clear_actions, v13_detail::ofp_instruction_actions
          >
    {
    public:
        static constexpr protocol::ofp_instruction_type instruction_type
            = protocol::OFPIT_CLEAR_ACTIONS;

        clear_actions() noexcept
            : instruction_actions_{
                  instruction_type
                , sizeof(raw_ofp_type)
                , { 0, 0, 0, 0 }
              }
        {
        }

    private:
        friend basic_instruction;

        explicit clear_actions(raw_ofp_type const& instruction_actions) noexcept
            : instruction_actions_(instruction_actions)
        {
        }

        auto ofp_instruction() const noexcept
            -> raw_ofp_type const&
        {
            return instruction_actions_;
        }

        static void validate_impl(clear_actions const&)
        {
        }

    private:
        raw_ofp_type instruction_actions_;
    };

    constexpr inline auto operator==(
            clear_actions const&, clear_actions const&) noexcept
        -> bool
    {
        return true;
    }

} // namespace instructions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTIONS_CLEAR_ACTIONS_HPP
