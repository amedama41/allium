#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP

#include <cstdint>
#include <boost/mpl/vector.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/placeholder.hpp>
#include <canard/network/protocol/openflow/v13/detail/type_erasure_concepts.hpp>

#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    using any_oxm_id = boost::type_erasure::any<
        boost::mpl::vector<
              boost::type_erasure::typeid_<>
            , boost::type_erasure::copy_constructible<>
            , has_oxm_type<std::uint32_t(), boost::type_erasure::_self const>
            , has_oxm_header<std::uint32_t(), boost::type_erasure::_self const>
            , has_oxm_has_mask<bool(), boost::type_erasure::_self const>
            , has_oxm_length<std::uint8_t(), boost::type_erasure::_self const>
            , has_length<std::uint16_t(), boost::type_erasure::_self const>
            , has_encode<std::vector<unsigned char>&(std::vector<unsigned char>&), boost::type_erasure::_self const>
        >
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_OXM_ID_HPP
