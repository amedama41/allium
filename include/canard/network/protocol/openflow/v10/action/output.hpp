#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <canard/network/protocol/openflow/v10/detail/action_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    class output
        : public v10_detail::action_adaptor<output, v10_detail::ofp_action_output>
    {
        using ofp_action_t = v10_detail::ofp_action_output;

    public:
        static ofp_action_type const action_type = OFPAT_OUTPUT;

        explicit output(
                  std::uint16_t const port
                , std::uint16_t const max_len = std::numeric_limits<std::uint16_t>::max())
            : output_{action_type, sizeof(ofp_action_t), port, max_len}
        {
        }

        auto port() const
            -> std::uint16_t
        {
            return output_.port;
        }

        auto max_length() const
            -> std::uint16_t
        {
            return output_.max_len;
        }

        static auto to_controller(std::uint16_t const max_len = std::numeric_limits<std::uint16_t>::max())
            -> output
        {
            return output{OFPP_CONTROLLER, max_len};
        }

    private:
        friend action_adaptor;

        auto ofp_action() const
            -> ofp_action_t const&
        {
            return output_;
        }

        explicit output(ofp_action_t const output)
            : output_(output)
        {
            if (output_.port == 0 || output_.port == OFPP_NONE) {
                throw std::runtime_error{"invalid outport"};
            }
        }

    private:
        ofp_action_t output_;
    };

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP
