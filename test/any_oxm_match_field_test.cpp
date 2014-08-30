#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE any_oxm_match_field_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <utility>
#include <boost/type_erasure/any_cast.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(any_oxm_match_field_test)

BOOST_AUTO_TEST_SUITE(assignment_test)

BOOST_AUTO_TEST_CASE(assign_to_same_type_test)
{
    auto sut = any_oxm_match_field{oxm_in_port{4}};

    sut = any_oxm_match_field{oxm_in_port{5}};

    BOOST_CHECK_EQUAL(boost::type_erasure::any_cast<oxm_in_port const&>(sut).oxm_value(), 5);
}

#if 0
BOOST_AUTO_TEST_CASE(assign_to_different_type_test)
{
    auto sut = any_oxm_match_field{oxm_in_port{4, 4}};
    auto phy_port = oxm_in_phy_port{5};
    auto copy = any_oxm_match_field{phy_port};

    copy = sut;

    BOOST_CHECK_EQUAL(copy.oxm_header(), phy_port.oxm_header());
    // BOOST_CHECK_EQUAL(boost::type_erasure::any_cast<oxm_in_port const&>(sut).oxm_value(), 5);
}
#endif

BOOST_AUTO_TEST_SUITE_END() // assignment_test

BOOST_AUTO_TEST_SUITE_END() // any_oxm_match_field_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
