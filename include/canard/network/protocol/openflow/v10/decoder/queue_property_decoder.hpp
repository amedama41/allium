#ifndef CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_DECODER_HPP

#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/queue_properties.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

struct queue_property_decoder
{
    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const prop_header
            = detail::decode<v10_detail::ofp_queue_prop_header>(it, last);

        switch (prop_header.property) {
#       define CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_CASE(z, N, _) \
        using property ## N \
            = std::tuple_element<N, default_queue_property_list>::type; \
        case property ## N::queue_property: \
            return function(property ## N::decode(first, last));
        static_assert(
                  std::tuple_size<default_queue_property_list>::value == 1
                , "not match to the number of queue property types");
        BOOST_PP_REPEAT(1, CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_CASE, _)
#       undef  CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_CASE
        default:
            throw std::runtime_error{"unknwon queue property"};
        }
    }
};

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_QUEUE_PROPERTY_DECODER_HPP
