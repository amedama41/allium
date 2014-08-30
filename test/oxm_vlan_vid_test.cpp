#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE oxm_vlan_vid_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(oxm_vlan_vid_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const vlan_vid = 1;

    auto const sut = oxm_vlan_vid{vlan_vid};

    BOOST_CHECK(not sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), sizeof(std::uint16_t));
    BOOST_CHECK_EQUAL(sut.oxm_value(), vlan_vid);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(std::uint32_t) + sut.oxm_length());
    BOOST_CHECK(not sut.wildcard());
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = oxm_vlan_vid{0x0fff};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(copy.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(copy.oxm_value(), sut.oxm_value());
    BOOST_CHECK_EQUAL(copy.oxm_mask(), sut.oxm_mask());
    BOOST_CHECK(copy.oxm_mask() == sut.oxm_mask());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE(specific_vlan_vid_test)

BOOST_AUTO_TEST_CASE(vlan_vid_is_OFPVID_NONE)
{
    auto const sut = oxm_vlan_vid{OFPVID_NONE};

    BOOST_CHECK(not sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_value(), OFPVID_NONE);
}

BOOST_AUTO_TEST_CASE(vlan_vid_is_OFPVID_PRESENT)
{
    auto const sut = oxm_vlan_vid{OFPVID_PRESENT};

    BOOST_CHECK(sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), sizeof(std::uint16_t) * 2);
    BOOST_CHECK_EQUAL(sut.oxm_value(), OFPVID_PRESENT);
    BOOST_REQUIRE(sut.oxm_mask());
    BOOST_CHECK_EQUAL(*sut.oxm_mask(), OFPVID_PRESENT);
}

BOOST_AUTO_TEST_CASE(vlan_vid_and_mask_are_OFPVID_PRESENT)
{
    auto const sut = oxm_vlan_vid{OFPVID_PRESENT, OFPVID_PRESENT};

    BOOST_CHECK(sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), sizeof(std::uint16_t) * 2);
    BOOST_CHECK_EQUAL(sut.oxm_value(), OFPVID_PRESENT);
    BOOST_REQUIRE(sut.oxm_mask());
    BOOST_CHECK_EQUAL(*sut.oxm_mask(), OFPVID_PRESENT);
}

BOOST_AUTO_TEST_CASE(vlan_vid_is_valid_with_OFPVID_PRESENT)
{
    auto const vlan_vid = 0x0fff;

    auto const sut = oxm_vlan_vid{vlan_vid | OFPVID_PRESENT};

    BOOST_CHECK(not sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_value(), vlan_vid);
}

BOOST_AUTO_TEST_SUITE_END() // specific_vlan_vid_test

BOOST_AUTO_TEST_SUITE(bad_vlan_vid_test)

BOOST_AUTO_TEST_CASE(vlan_vid_is_too_large)
{
    BOOST_CHECK_THROW(oxm_vlan_vid{0x2000}, std::runtime_error);
    BOOST_CHECK_THROW(oxm_vlan_vid{0xffff}, std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // bad_vlan_vid_test

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_vlan_vid{459};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_vlan_vid::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_CASE(encode_decode_test_when_has_mask)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_vlan_vid{0x35, 0x12};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_vlan_vid::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_REQUIRE(decoded_field.oxm_mask() && sut.oxm_mask());
    BOOST_CHECK_EQUAL(*decoded_field.oxm_mask(), *sut.oxm_mask());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE_END() // oxm_vlan_vid_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
