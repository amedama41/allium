#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/endian/conversion.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(byteorder_test)

BOOST_AUTO_TEST_CASE(ofp_header_hton_test)
{
    auto const header = v13_detail::ofp_header{protocol::OFP_VERSION, protocol::OFPT_HELLO, 0x3456, 0x12345678};
    auto const norder_header = boost::endian::native_to_big(header);
    BOOST_CHECK_EQUAL(norder_header.version, protocol::OFP_VERSION);
    BOOST_CHECK_EQUAL(norder_header.type, protocol::OFPT_HELLO);
    BOOST_CHECK_EQUAL(norder_header.length, 0x5634);
    BOOST_CHECK_EQUAL(norder_header.xid, 0x78563412);
}

BOOST_AUTO_TEST_CASE(ofp_header_ntoh_test)
{
    auto const header = v13_detail::ofp_header{protocol::OFP_VERSION, protocol::OFPT_HELLO, 0x3456, 0x12345678};
    auto const horder_header = boost::endian::big_to_native(header);
    BOOST_CHECK_EQUAL(horder_header.version, protocol::OFP_VERSION);
    BOOST_CHECK_EQUAL(horder_header.type, protocol::OFPT_HELLO);
    BOOST_CHECK_EQUAL(horder_header.length, 0x5634);
    BOOST_CHECK_EQUAL(horder_header.xid, 0x78563412);
}

BOOST_AUTO_TEST_SUITE_END() // byteorder_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
