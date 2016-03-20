#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_DECODER_HPP

#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/decoder/set_field_decoder.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

struct action_decoder
{
    using ofp_action_type = protocol::ofp_action_type;
    using action_type_list = default_all_action_list;
    using non_set_field_action_type_list = default_action_list;
    static_assert(
              std::tuple_size<non_set_field_action_type_list>::value == 15
            , "not match to the number of action types");

    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const action_header
            = detail::decode<v13_detail::ofp_action_header>(it, last);

        if (std::distance(first, last) < action_header.len) {
            throw std::runtime_error{"action length is too big"};
        }

        switch (action_header.type) {
#       define CANARD_NETWORK_OPENFLOW_V13_ACTION_CASE(z, N, _) \
        using action ## N \
            = std::tuple_element<N, non_set_field_action_type_list>::type; \
        case action ## N::action_type: \
            return function(action ## N::decode(first, last));
        BOOST_PP_REPEAT(15, CANARD_NETWORK_OPENFLOW_V13_ACTION_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_ACTION_CASE
        case protocol::OFPAT_SET_FIELD:
            return set_field_decoder::decode<ReturnType>(
                    first, last, std::move(function));
        default:
            throw std::runtime_error{"unknwon action type"};
        }
    }
};

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_DECODER_HPP
