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
BOOST_AUTO_TEST_SUITE(in_phy_port_test)

    BOOST_DATA_TEST_CASE(
              no_mask_create_success_test
            , bdata::make(std::vector<std::uint32_t>{1, proto::OFPP_MAX})
            , value)
    {
        auto const sut = match::in_phy_port::create(value);

        BOOST_TEST(sut.oxm_length() == sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST(!sut.oxm_has_mask());
        BOOST_TEST(sut.oxm_mask() == boost::none);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              has_mask_create_success_test
            , bdata::make(std::vector<std::uint32_t>{1, proto::OFPP_MAX})
            ^ bdata::make(std::vector<std::uint32_t>{1, proto::OFPP_MAX})
            , value, mask)
    {
        auto const sut = match::in_phy_port::create(value, mask);

        BOOST_TEST(sut.oxm_length() == 2 * sizeof(std::uint32_t));
        BOOST_TEST(sut.oxm_value() == value);
        BOOST_TEST_REQUIRE(sut.oxm_has_mask());
        BOOST_TEST(*sut.oxm_mask() == mask);
        BOOST_TEST(sut.length()
                == sizeof(std::uint32_t) + 2 * sizeof(std::uint32_t));
        BOOST_TEST(!sut.is_wildcard());
        BOOST_TEST(!sut.is_exact());
    }

    BOOST_DATA_TEST_CASE(
              no_mask_create_failure_test
            , bdata::make(std::vector<std::uint32_t>{
                0, proto::OFPP_MAX + 1, proto::OFPP_ANY
              })
            , value)
    {
        BOOST_CHECK_THROW(
                match::in_phy_port::create(value), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(has_mask_create_failure_test)
    {
        BOOST_CHECK_THROW(
                match::in_phy_port::create(0x0101, 0x1010), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // in_phy_port_test
BOOST_AUTO_TEST_SUITE_END() // oxm_match_test
