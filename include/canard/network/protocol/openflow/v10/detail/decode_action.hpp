#ifndef CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_HPP

#include <stdexcept>
#include <tuple>
#include <boost/format.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v10/actions.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace v10_detail {

    template <class ReturnType, class Iterator, class Function>
    auto decode_action(Iterator& first, Iterator last, Function&& func)
        -> ReturnType
    {
        auto first_copy = first;
        auto const action_header
            = detail::decode<v10_detail::ofp_action_header>(first_copy, last);

        switch (action_header.type) {
#       define CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_CASE(z, N, _) \
        case std::tuple_element<N, default_action_list>::type::action_type: \
            return func(std::tuple_element<N, default_action_list>::type::decode(first, last));
        BOOST_PP_REPEAT(1, CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_CASE
        default:
            throw std::runtime_error{
                (boost::format{"unknwon action type(%1%)"} % action_header.type).str()
            };
        }
    }

} // namespace v10_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_DECODE_ACTION_HPP
