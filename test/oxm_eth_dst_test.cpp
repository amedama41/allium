#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <canard/mac_address.hpp>
#include <canard/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(oxm_eth_dst_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const sut = oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}};

    BOOST_CHECK(not sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), 6);
    BOOST_CHECK(not sut.oxm_mask());
    BOOST_CHECK_EQUAL(sut.length(), sizeof(std::uint32_t) + sut.oxm_length());
    BOOST_CHECK(not sut.wildcard());
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = oxm_eth_dst{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(copy.oxm_length(), sut.oxm_length());
    CANARD_CHECK_EQUAL_COLLECTIONS(copy.oxm_value(), sut.oxm_value());
    BOOST_CHECK(copy.oxm_mask() == sut.oxm_mask());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE(value_category_test)

BOOST_AUTO_TEST_CASE(construct_by_lvalue)
{
    auto addr = std::array<std::uint8_t, 6>{{0x11, 0x22, 0x33, 0x44, 0x55, 0x66}};

    auto const sut = oxm_eth_dst{addr};

    CANARD_CHECK_EQUAL_COLLECTIONS(sut.oxm_value(), addr);
}

BOOST_AUTO_TEST_CASE(construct_by_const_lvalue)
{
    auto const addr = std::array<std::uint8_t, 6>{{0x12, 0x23, 0x34, 0x45, 0x56, 0x67}};

    auto const sut = oxm_eth_dst{addr};

    CANARD_CHECK_EQUAL_COLLECTIONS(sut.oxm_value(), addr);
}

BOOST_AUTO_TEST_CASE(construct_by_xvalue)
{
    auto addr = std::array<std::uint8_t, 6>{{0x21, 0x22, 0x23, 0x24, 0x25, 0x26}};

    auto const sut = oxm_eth_dst{std::move(addr)};

    CANARD_CHECK_EQUAL_COLLECTIONS(sut.oxm_value(), addr);
}

BOOST_AUTO_TEST_CASE(construct_by_pvalue)
{
    auto const sut = oxm_eth_dst{{{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}};

    CANARD_CHECK_EQUAL_COLLECTIONS(sut.oxm_value(), (oxm_eth_dst::value_type{{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}));
}

BOOST_AUTO_TEST_CASE(construct_by_lvalue_mac_address)
{
    auto addr = canard::mac_address{{{0x31, 0x32, 0x33, 0x34, 0x35, 0x36}}};

    auto const sut = oxm_eth_dst{addr};

    BOOST_CHECK_EQUAL(canard::mac_address{sut.oxm_value()}, addr);
}

BOOST_AUTO_TEST_CASE(construct_by_const_lvalue_mac_address)
{
    auto const addr = canard::mac_address{{{0x31, 0x32, 0x33, 0x34, 0x35, 0x36}}};

    auto const sut = oxm_eth_dst{addr};

    BOOST_CHECK_EQUAL(canard::mac_address{sut.oxm_value()}, addr);
}

BOOST_AUTO_TEST_CASE(construct_by_xvalue_mac_address)
{
    auto addr = canard::mac_address{{{0x31, 0x32, 0x33, 0x34, 0x35, 0x36}}};

    auto const sut = oxm_eth_dst{std::move(addr)};

    BOOST_CHECK_EQUAL(canard::mac_address{sut.oxm_value()}, addr);
}

BOOST_AUTO_TEST_CASE(construct_by_prvalue_mac_address)
{
    auto const sut = oxm_eth_dst{canard::mac_address{{{0x31, 0x32, 0x33, 0x34, 0x35, 0x36}}}};

    BOOST_CHECK_EQUAL(canard::mac_address{sut.oxm_value()}, (canard::mac_address{{{0x31, 0x32, 0x33, 0x34, 0x35, 0x36}}}));
}

BOOST_AUTO_TEST_SUITE_END() // value_category_test

BOOST_AUTO_TEST_CASE(has_mask)
{
    auto const mask = oxm_eth_dst::value_type{{0x01, 0x02, 0x04, 0x10, 0x20, 0x40}};

    auto const sut = oxm_eth_dst{{{0x11, 0x22, 0x36, 0x54, 0x65, 0x66}}, mask};

    BOOST_CHECK(sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), 6 * 2);
    BOOST_REQUIRE(sut.oxm_mask());
    CANARD_CHECK_EQUAL_COLLECTIONS(*sut.oxm_mask(), mask);
    BOOST_CHECK(not sut.wildcard());
}

BOOST_AUTO_TEST_CASE(field_is_wildcard)
{
    auto const sut = oxm_eth_dst{{{0x11, 0x22, 0x33, 0x44, 0x55, 0x66}}, {{0, 0, 0, 0, 0, 0}}};

    BOOST_CHECK(sut.wildcard());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_eth_dst{{{0x11, 0x22, 0x36, 0x54, 0x65, 0x66}}};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_eth_dst::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    CANARD_CHECK_EQUAL_COLLECTIONS(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_CASE(encode_decode_test_when_has_mask)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_eth_dst{{{0x11, 0x22, 0x36, 0x54, 0x65, 0x66}}, {{0x01, 0x02, 0x04, 0x10, 0x20, 0x40}}};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_eth_dst::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    CANARD_CHECK_EQUAL_COLLECTIONS(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_REQUIRE(decoded_field.oxm_mask() && sut.oxm_mask());
    CANARD_CHECK_EQUAL_COLLECTIONS(*decoded_field.oxm_mask(), *sut.oxm_mask());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE_END() // oxm_eth_dst_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
