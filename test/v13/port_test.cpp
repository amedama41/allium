#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/port.hpp>
#include <boost/test/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(port_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    // auto const sut = port{};
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE_END() // port_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
