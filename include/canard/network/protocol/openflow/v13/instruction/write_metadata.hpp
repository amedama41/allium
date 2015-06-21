#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_METADATA_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_METADATA_HPP

#include <cstdint>
#include <limits>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace instructions {

        class write_metadata
        {
        public:
            static protocol::ofp_instruction_type const instruction_type
                = protocol::OFPIT_WRITE_METADATA;

            explicit write_metadata(std::uint64_t const metadata)
                : write_metadata{metadata, std::numeric_limits<std::uint64_t>::max()}
            {
            }

            write_metadata(std::uint64_t const metadata, std::uint64_t const metadata_mask)
                : write_metadata_{
                      instruction_type, sizeof(v13_detail::ofp_instruction_write_metadata), {0}
                    , metadata, metadata_mask
                }
            {
            }

            auto type() const
                -> protocol::ofp_instruction_type
            {
                return instruction_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_instruction_write_metadata);
            }

            auto metadata() const
                -> std::uint64_t
            {
                return write_metadata_.metadata;
            }

            auto metadata_mask() const
                -> std::uint64_t
            {
                return write_metadata_.metadata_mask;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, write_metadata_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> write_metadata
            {
                auto const instruction_write_metadata = detail::decode<v13_detail::ofp_instruction_write_metadata>(first, last);
                if (instruction_write_metadata.type != instruction_type) {
                    throw 1;
                }
                if (instruction_write_metadata.len != sizeof(v13_detail::ofp_instruction_write_metadata)) {
                    throw 2;
                }
                return {instruction_write_metadata.metadata, instruction_write_metadata.metadata_mask};
            }

        private:
            v13_detail::ofp_instruction_write_metadata write_metadata_;
        };

    } // namespace instructions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_WRITE_METADATA_HPP
