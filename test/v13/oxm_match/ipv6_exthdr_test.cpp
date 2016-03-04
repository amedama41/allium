#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <stdexcept>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace match = v13::oxm_match;

using proto = v13::protocol;

namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(oxm_match_test)
BOOST_AUTO_TEST_SUITE(ipv6_exthdr_test)

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<std::uint16_t>{0x0000, 0x01ff})
            , value)
    {
        auto const sut = match::ipv6_exthdr::create(value);

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              has_mask_create_success_test
            , bdata::make(std::vector<std::uint16_t>{0x0000, 0x01ff})
            ^ bdata::make(std::vector<std::uint16_t>{0x0001, 0x0fff})
            , value, mask)
    {
        auto const sut = match::ipv6_exthdr::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint16_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint16_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_failure_test
            , bdata::make(std::vector<std::uint16_t>{0x1000, 0xffff})
            , value)
    {
        BOOST_CHECK_THROW(
                match::ipv6_exthdr::create(value), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                match::ipv6_exthdr::create(0x01, 0x00), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // ipv6_exthdr_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
