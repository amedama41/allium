#ifndef CANARD_NETWORK_OPENFLOW_V13_TYPE_ERASURE_CONCEPTS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TYPE_ERASURE_CONCEPTS_HPP

#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/free.hpp>

BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_type), type, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_length), length, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_encode), encode, 1)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_oxm_type), oxm_type, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_oxm_header), oxm_header, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_oxm_has_mask), oxm_has_mask, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_oxm_length), oxm_length, 0)
BOOST_TYPE_ERASURE_MEMBER((canard)(network)(openflow)(v13)(has_wildcard), wildcard, 0)
BOOST_TYPE_ERASURE_FREE((canard)(network)(openflow)(v13)(has_action_order), action_order, 1)
BOOST_TYPE_ERASURE_FREE((canard)(network)(openflow)(v13)(has_instruction_order), instruction_order, 1)

#endif // CANARD_NETWORK_OPENFLOW_V13_TYPE_ERASURE_CONCEPTS_HPP
