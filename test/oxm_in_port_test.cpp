#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/optional/optional_io.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(oxm_in_port_test)

BOOST_AUTO_TEST_CASE(oxm_type_definition_test)
{
    BOOST_CHECK((std::is_same<oxm_in_port::value_type, std::uint32_t>::value));
    BOOST_CHECK((std::is_same<oxm_in_port::needs_byteorder_conversion, std::true_type>::value));
    BOOST_CHECK_EQUAL(oxm_in_port::oxm_class(), protocol::OFPXMC_OPENFLOW_BASIC);
    BOOST_CHECK_EQUAL(oxm_in_port::oxm_field(), protocol::OFPXMT_OFB_IN_PORT);
    BOOST_CHECK_EQUAL(oxm_in_port::oxm_type(), protocol::OFPXMC_OPENFLOW_BASIC << 7 | protocol::OFPXMT_OFB_IN_PORT);
}

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const port_no = 1U;
    auto const sut = oxm_in_port{port_no};

    BOOST_CHECK(not sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(sut.oxm_length(), sizeof(std::uint32_t));
    BOOST_CHECK_EQUAL(sut.oxm_value(), port_no);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(std::uint32_t) + sut.oxm_length());
    BOOST_CHECK(not sut.wildcard());
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = oxm_in_port{32};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(copy.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(copy.oxm_value(), sut.oxm_value());
    BOOST_CHECK_EQUAL(copy.oxm_mask(), sut.oxm_mask());
    BOOST_CHECK(copy.oxm_mask() == sut.oxm_mask());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE(value_category_test)

BOOST_AUTO_TEST_CASE(construct_by_lvalue)
{
    auto port_no = std::uint32_t{1U};

    auto const sut = oxm_in_port{port_no};

    BOOST_CHECK_EQUAL(sut.oxm_value(), port_no);
}

BOOST_AUTO_TEST_CASE(construct_by_const_lvalue)
{
    auto const port_no = std::uint32_t{1U};

    auto const sut = oxm_in_port{port_no};

    BOOST_CHECK_EQUAL(sut.oxm_value(), port_no);
}

BOOST_AUTO_TEST_CASE(construct_by_xvalue)
{
    auto port_no = std::uint32_t{1U};

    auto const sut = oxm_in_port{std::move(port_no)};

    BOOST_CHECK_EQUAL(sut.oxm_value(), port_no);
}

BOOST_AUTO_TEST_CASE(construct_by_prvalue)
{
    auto const sut = oxm_in_port{std::uint32_t{protocol::OFPP_MAX}};

    BOOST_CHECK_EQUAL(sut.oxm_value(), protocol::OFPP_MAX);
}

BOOST_AUTO_TEST_SUITE_END() // value_category_test

BOOST_AUTO_TEST_CASE(port_no_is_zero)
{
    BOOST_CHECK_THROW(oxm_in_port{0}, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(port_no_is_not_physical_and_logical_port)
{
    BOOST_CHECK_THROW(oxm_in_port{protocol::OFPP_MAX + 1}, std::runtime_error);
    BOOST_CHECK_THROW(oxm_in_port{protocol::OFPP_ANY}, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(field_is_wildcard)
{
    auto const sut = oxm_in_port{3000, 0};

    BOOST_CHECK(sut.wildcard());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_in_port{459};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_in_port::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_CASE(encode_decode_test_when_has_mask)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = oxm_in_port{0x66, 0x22};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_field = oxm_in_port::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_field.oxm_has_mask(), sut.oxm_has_mask());
    BOOST_CHECK_EQUAL(decoded_field.oxm_length(), sut.oxm_length());
    BOOST_CHECK_EQUAL(decoded_field.oxm_value(), sut.oxm_value());
    BOOST_REQUIRE(decoded_field.oxm_mask() && sut.oxm_mask());
    BOOST_CHECK_EQUAL(*decoded_field.oxm_mask(), *sut.oxm_mask());
    BOOST_CHECK_EQUAL(decoded_field.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_field.wildcard(), sut.wildcard());
}

BOOST_AUTO_TEST_SUITE_END() // oxm_in_port_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
