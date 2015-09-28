#ifndef CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_HPP

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/endian/conversion.hpp>
#include <boost/format.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <class ReturnType, class Iterator, class Function>
        auto decode_instruction(Iterator& first, Iterator last, Function func)
            -> ReturnType
        {
            auto instruction = v13_detail::ofp_instruction{};
            std::copy_n(first, sizeof(instruction), canard::as_byte_range(instruction).begin());

            static_assert(std::tuple_size<default_instruction_list>::value == 6, "");
            switch (boost::endian::big_to_native(instruction.type)) {
#           define CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE(z, N, _) \
            case std::tuple_element<N, default_instruction_list>::type::instruction_type: \
                return func(std::tuple_element<N, default_instruction_list>::type::decode(first, last));
            BOOST_PP_REPEAT(6, CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE, _)
#           undef CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_CASE

            default:
                std::advance(first, boost::endian::big_to_native(instruction.len));
                throw std::runtime_error{(boost::format{"%1%: unknown instruction type %2%"}
                        % __func__ % (boost::endian::big_to_native(instruction.type))).str()
                };
            }
        }

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DECODE_INSTRUCTION_HPP
