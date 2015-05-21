#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    class output
    {
    public:
        static ofp_action_type const action_type = OFPAT_OUTPUT;

        explicit output(
                  std::uint16_t const port
                , std::uint16_t const max_len = std::numeric_limits<std::uint16_t>::max())
            : output_{action_type, sizeof(v10_detail::ofp_action_output), port, max_len}
        {
        }

        auto type() const
            -> ofp_action_type
        {
            return action_type;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(v10_detail::ofp_action_output);
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

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, output_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> output
        {
            auto const output = detail::decode<v10_detail::ofp_action_output>(first, last);
            return actions::output{output};
        }

        static auto to_controller(std::uint16_t const max_len = std::numeric_limits<std::uint16_t>::max())
            -> output
        {
            return output{OFPP_CONTROLLER, max_len};
        }

    private:
        explicit output(v10_detail::ofp_action_output const output)
            : output_(output)
        {
            if (output_.type != action_type) {
                throw std::runtime_error{"invalid action type"};
            }
            if (output_.len != sizeof(v10_detail::ofp_action_output)) {
                throw std::runtime_error{"invalid length"};
            }
            if (output_.port == 0 || output_.port == OFPP_NONE) {
                throw std::runtime_error{"invalid outport"};
            }
        }

    private:
        v10_detail::ofp_action_output output_;
    };

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_OUTPUT_HPP
