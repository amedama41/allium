#ifndef CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_DECODER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_DECODER_HPP

#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/min_base_size_element.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/queue_properties.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

struct queue_property_decoder
{
    template <class ReturnType, class Iterator, class Function>
    static auto decode(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto it = first;
        auto const prop_header
            = detail::decode<v13_detail::ofp_queue_prop_header>(it, last);

        if (std::distance(first, last) < prop_header.len) {
            throw std::runtime_error{"queue property length is too big"};
        }

        switch (prop_header.property) {
#       define CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_CASE(z, N, _) \
        using property ## N \
            = std::tuple_element<N, default_queue_property_list>::type; \
        case property ## N::queue_property: \
            return function(property ## N::decode(first, last));
        static_assert(
                  std::tuple_size<default_queue_property_list>::value == 2
                , "not match to the number of queue property types");
        BOOST_PP_REPEAT(2, CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_CASE, _)
#       undef  CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_CASE
        default:
            throw std::runtime_error{"unknwon queue property"};
        }
    }
};

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_QUEUE_PROPERTY_DECODER_HPP
