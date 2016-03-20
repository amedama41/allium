#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODER_HPP

#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

struct oxm_match_field_decoder
{
    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const oxm_header = detail::decode<std::uint32_t>(it, last);

        if (std::distance(it, last) < (oxm_header & 0xff)) {
            throw std::runtime_error{"oxm length is too big"};
        }

        switch (oxm_header >> 9) {
#       define CANARD_NETWORK_OPENFLOW_V13_MATCH_FIELD_CASE(z, N, _) \
        using oxm_match_field ## N \
            = std::tuple_element<N, default_oxm_match_field_list>::type; \
        case oxm_match_field ## N::oxm_type(): \
            return function(oxm_match_field ## N::decode(first, last));
        static_assert(
                  std::tuple_size<default_oxm_match_field_list>::value == 40
                , "not match to the number of oxm match types");
        BOOST_PP_REPEAT(40, CANARD_NETWORK_OPENFLOW_V13_MATCH_FIELD_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_MATCH_FIELD_CASE
        default:
            throw std::runtime_error{"unknwon oxm type"};
        }
    }
};

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODER_HPP
