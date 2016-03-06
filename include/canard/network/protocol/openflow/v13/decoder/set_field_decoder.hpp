#ifndef CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_DECODER_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/action/set_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
struct set_field_decoder
{
    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const set_field
            = detail::decode<v13_detail::ofp_action_set_field>(it, last);

        auto const oxm_header
            = actions::basic_set_field::extract_oxm_header(set_field);

        static_assert(
                  std::tuple_size<default_set_field_list>::value == 36
                , "not match to the number of set_field types");
        switch (oxm_header >> 9) {
#       define CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_CASE(z, N, _) \
        using set_field ## N = \
            std::tuple_element<N, default_set_field_list>::type; \
        case set_field ## N::oxm_type(): \
            return function(set_field ## N::decode(first, last));
        BOOST_PP_REPEAT(36, CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_CASE
        default:
            throw std::runtime_error{"unknwon set field oxm type"};
        }
    }
};

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SET_FIELD_DECODER_HPP
