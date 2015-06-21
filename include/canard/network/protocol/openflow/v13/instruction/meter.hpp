#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_METER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_METER_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class meter
        {
        public:
            static protocol::ofp_instruction_type const instruction_type
                = protocol::OFPIT_METER;

            explicit meter(std::uint32_t const meter_id)
                : meter_{instruction_type, sizeof(v13_detail::ofp_instruction_meter), meter_id}
            {
                if (meter_id == 0 || meter_id > protocol::OFPM_MAX) {
                    throw 3;
                }
            }

            auto type() const
                -> protocol::ofp_instruction_type
            {
                return instruction_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_instruction_meter);
            }

            auto meter_id() const
                -> std::uint32_t
            {
                return meter_.meter_id;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, meter_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> meter
            {
                auto const instruction_meter = detail::decode<v13_detail::ofp_instruction_meter>(first, last);
                if (instruction_meter.type != instruction_type) {
                    throw 1;
                }
                if (instruction_meter.len != sizeof(v13_detail::ofp_instruction_meter)) {
                    throw 2;
                }
                return meter{instruction_meter.meter_id};
            }

        private:
            v13_detail::ofp_instruction_meter meter_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_METER_HPP
