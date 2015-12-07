#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_OUTPUT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_OUTPUT_HPP

#include <cstdint>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class output
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_OUTPUT;

            explicit output(std::uint32_t const port)
                : output_{
                    action_type, length(), port, protocol::OFPCML_NO_BUFFER, {0}
                  }
            {
                if (!validate_output_port(port)) {
                    throw 1;
                }
            }

            output(std::uint32_t const port, std::uint16_t const max_length)
                : output_{action_type, length(), port, max_length, {0}}
            {
                if (!validate_output_port(port)) {
                    throw 1;
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
                return sizeof(v13_detail::ofp_action_output);
            }

            auto port() const
                -> protocol::ofp_port_no
            {
                return protocol::ofp_port_no(output_.port);
            }

            auto max_length() const
                -> protocol::ofp_controller_max_len
            {
                return protocol::ofp_controller_max_len(output_.max_len);
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return v13_detail::encode(container, output_);
            }

        private:
            explicit output(v13_detail::ofp_action_output const& action_output)
                : output_(action_output)
            {
                if (output_.type != action_type) {
                    throw 1;
                }
                if (output_.len != sizeof(v13_detail::ofp_action_output)) {
                    throw 2;
                }
                if (!validate_output_port(output_.port)) {
                    throw std::runtime_error{(boost::format{"%1%: port(%2%) is invalid"} % __func__ % output_.port).str()};
                }
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> output
            {
                auto const action_output = v13_detail::decode<v13_detail::ofp_action_output>(first, last);
                return output{action_output};
            }

            static auto to_controller(
                    std::uint16_t const max_length = protocol::OFPCML_NO_BUFFER)
                -> output
            {
                return output{protocol::OFPP_CONTROLLER, max_length};
            }

        private:
            static auto validate_output_port(std::uint32_t const port)
                -> bool
            {
                return port != 0 && port != protocol::OFPP_ANY;
            }

            v13_detail::ofp_action_output output_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_OUTPUT_HPP
