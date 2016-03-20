#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_DECODER_HPP

#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

struct instruction_decoder
{
    using ofp_instruction_type = protocol::ofp_instruction_type;
    using instruction_type_list = default_instruction_list;
    static_assert(
              std::tuple_size<instruction_type_list>::value == 6
            , "not match to the number of instruction types");

    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const instruction
            = detail::decode<v13_detail::ofp_instruction>(it, last);

        if (std::distance(first, last) < instruction.len) {
            throw std::runtime_error{"instruction length is too big"};
        }

        switch (instruction.type) {
#       define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CASE(z, N, _) \
        using instruction ## N \
            = std::tuple_element<N, instruction_type_list>::type; \
        case instruction ## N::instruction_type: \
            return function(instruction ## N::decode(first, last));
        BOOST_PP_REPEAT(6, CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_CASE
        default:
            throw std::runtime_error{"unknown instruction type"};
        }
    }
};

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_DECODER_HPP
