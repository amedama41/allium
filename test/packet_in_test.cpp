#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE packet_in_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/message/packet_in.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(packet_in_test)



BOOST_AUTO_TEST_SUITE_END() // packet_in_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
