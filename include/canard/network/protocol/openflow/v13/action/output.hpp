#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_OUTPUT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_OUTPUT_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/v13/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    class output
        : public detail::v13::basic_action<
            output, v13_detail::ofp_action_output
          >
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_OUTPUT;

        explicit output(
                  std::uint32_t const port_no
                , std::uint16_t const max_length
                    = protocol::OFPCML_NO_BUFFER) noexcept
            : action_output_{
                  action_type
                , length()
                , port_no
                , max_length
                , { 0, 0, 0, 0, 0, 0 }
              }
        {
        }

        auto port_no() const noexcept
            -> std::uint32_t
        {
            return action_output_.port;
        }

        auto max_length() const noexcept
            -> std::uint16_t
        {
            return action_output_.max_len;
        }

        static auto to_controller(
                std::uint16_t const max_length
                    = protocol::OFPCML_NO_BUFFER) noexcept
            -> output
        {
            return output{protocol::OFPP_CONTROLLER, max_length};
        }

    private:
        friend basic_action;

        explicit output(raw_ofp_type const& action_output) noexcept
            : action_output_(action_output)
        {
        }

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return action_output_;
        }

        static void validate_impl(output const& action)
        {
            if (action.port_no() == 0
                    || action.port_no() == protocol::OFPP_ANY) {
                throw std::runtime_error{"invalid port_no"};
            }
            if (action.max_length() > protocol::OFPCML_MAX
                    && action.max_length() != protocol::OFPCML_NO_BUFFER) {
                throw std::runtime_error{"invalid max_length"};
            }
        }

    private:
        raw_ofp_type action_output_;
    };

    inline auto operator==(output const& lhs, output const& rhs) noexcept
        -> bool
    {
        return lhs.port_no() == rhs.port_no()
            && lhs.max_length() == rhs.max_length();
    }

} // namespace actions
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_OUTPUT_HPP
