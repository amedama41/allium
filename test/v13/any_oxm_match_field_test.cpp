#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <utility>
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

    BOOST_CHECK_EQUAL(any_cast<oxm_in_port>(sut).oxm_value(), 5);
}

BOOST_AUTO_TEST_CASE(assign_to_different_type_test)
{
    auto sut = any_oxm_match_field{oxm_in_port{4, 4}};
    auto copy = any_oxm_match_field{oxm_in_phy_port{5}};

    copy = sut;

    BOOST_CHECK_EQUAL(copy.oxm_type(), oxm_in_port::oxm_type());
    BOOST_CHECK_EQUAL(any_cast<oxm_in_port>(copy).oxm_value(), 4);
    BOOST_CHECK_EQUAL(any_cast<oxm_in_port>(copy).oxm_mask().get(), 4);
}

BOOST_AUTO_TEST_SUITE_END() // assignment_test

BOOST_AUTO_TEST_SUITE_END() // any_oxm_match_field_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
