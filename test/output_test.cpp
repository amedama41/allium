#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE output_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/action/output.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(output_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const port = std::uint32_t{1};
    auto const sut = actions::output{port};

    BOOST_CHECK_EQUAL(sut.type(), OFPAT_OUTPUT);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(detail::ofp_action_output));
    BOOST_CHECK_EQUAL(sut.port(), port);
    BOOST_CHECK_EQUAL(sut.max_length(), OFPCML_NO_BUFFER);
    BOOST_CHECK_EQUAL(sut.length() % 8, 0);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = actions::output{OFPP_MAX, 0};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.port(), sut.port());
    BOOST_CHECK_EQUAL(copy.max_length(), sut.max_length());
}

BOOST_AUTO_TEST_SUITE(bad_port_no_test)

BOOST_AUTO_TEST_CASE(port_no_is_zero)
{
    auto const invalid_port = 0;
    BOOST_CHECK_THROW(actions::output{invalid_port}, int);
}

BOOST_AUTO_TEST_CASE(port_is_any)
{
    auto const port = OFPP_ANY;
    BOOST_CHECK_THROW(actions::output{port}, int);
}

BOOST_AUTO_TEST_SUITE_END() // bad_port_no_test

BOOST_AUTO_TEST_CASE(to_controller_factory_test)
{
    auto const sut = actions::output::to_controller(OFPCML_MAX);

    BOOST_CHECK_EQUAL(sut.type(), OFPAT_OUTPUT);
    BOOST_CHECK_EQUAL(sut.length(), sizeof(detail::ofp_action_output));
    BOOST_CHECK_EQUAL(sut.port(), OFPP_CONTROLLER);
    BOOST_CHECK_EQUAL(sut.max_length(), OFPCML_MAX);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = actions::output{OFPP_IN_PORT};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_action = actions::output::decode(it, buffer.end());

    BOOST_CHECK(it == buffer.end());
    BOOST_CHECK_EQUAL(decoded_action.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_action.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_action.port(), sut.port());
    BOOST_CHECK_EQUAL(decoded_action.max_length(), sut.max_length());
}

BOOST_AUTO_TEST_SUITE_END() // output_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
