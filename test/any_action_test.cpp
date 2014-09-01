#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/any_action.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <boost/type_erasure/any_cast.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(any_action_test)

BOOST_AUTO_TEST_SUITE(assignment_test)

#if 0
BOOST_AUTO_TEST_CASE(copy_assign_test)
{
    auto sut = any_action{actions::set_field{oxm_in_port{4}}};
    auto copy = any_action{actions::set_field{oxm_in_port{5}}};

    copy = sut;

    BOOST_CHECK_EQUAL(boost::type_erasure::any_cast<oxm_in_port const&>(
                boost::type_erasure::any_cast<actions::set_field const&>(copy).oxm_match_field()).oxm_value()
            , 4
    );
}

BOOST_AUTO_TEST_CASE(move_assign_test)
{
    auto sut = any_action{actions::set_field{oxm_in_port{4}}};
    auto copy = any_action{actions::set_field{oxm_in_port{5}}};

    copy = std::move(sut);

    BOOST_CHECK_EQUAL(boost::type_erasure::any_cast<oxm_in_port const&>(
                boost::type_erasure::any_cast<actions::set_field const&>(copy).oxm_match_field()).oxm_value()
            , 4
    );
}
#endif

BOOST_AUTO_TEST_SUITE_END() // assignment_test


BOOST_AUTO_TEST_SUITE(container_test)

#if 0
BOOST_AUTO_TEST_CASE(vector_copy_assign_test)
{
    auto sut = std::vector<any_action>{
        actions::set_field{oxm_in_port{4}}, actions::set_field{oxm_eth_type{0x0800}}
    };
    auto copy = std::vector<any_action>{
        actions::set_field{oxm_eth_type{0x0800}}, actions::set_field{oxm_in_port{4}}
    };

    auto tmp = sut;
    copy.swap(tmp);

    auto const action1 = boost::type_erasure::any_cast<actions::set_field*>(&copy[0]);
    BOOST_REQUIRE(action1);
    BOOST_CHECK_NE(action1->oxm_match_field().oxm_type(), oxm_eth_type::oxm_type());
    BOOST_CHECK_EQUAL(action1->oxm_match_field().oxm_type(), oxm_in_port::oxm_type());
}
#endif

BOOST_AUTO_TEST_SUITE_END() // container_test

BOOST_AUTO_TEST_SUITE_END() // any_action_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
