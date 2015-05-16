#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>
#include <boost/test/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(hello_elements_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(one_bitmap_test)
    {
        auto sut = hello_elements::versionbitmap{std::vector<std::uint32_t>{0x00000012}};

        BOOST_CHECK_EQUAL(sut.type(), OFPHET_VERSIONBITMAP);
        BOOST_CHECK_EQUAL(sut.length(), 8);
    }

    BOOST_AUTO_TEST_CASE(multi_bitmaps_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000012, 0x00010001}};

        BOOST_CHECK_EQUAL(sut.type(), OFPHET_VERSIONBITMAP);
        BOOST_CHECK_EQUAL(sut.length(), 12);
    }

    BOOST_AUTO_TEST_CASE(all_zero_bitmaps_test)
    {
        BOOST_CHECK_THROW((hello_elements::versionbitmap{{0x00000000, 0x00000000}}), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(empty_bitmap_vector_test)
    {
        BOOST_CHECK_THROW((hello_elements::versionbitmap{{}}), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(version_support_test)

    BOOST_AUTO_TEST_CASE(of13_support_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000010}};

        BOOST_CHECK(sut.support(v13::OFP_VERSION));
    }

    BOOST_AUTO_TEST_CASE(of11_and_13_support_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000012}};

        auto const V10_OFP_VERSION = 0x1;
        BOOST_CHECK(sut.support(V10_OFP_VERSION));
        BOOST_CHECK(sut.support(v13::OFP_VERSION));
    }

    BOOST_AUTO_TEST_CASE(multi_bitmaps_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000012, 0x00010001}};

        auto const V10_OFP_VERSION = 0x1;
        BOOST_CHECK(sut.support(V10_OFP_VERSION));
        BOOST_CHECK(sut.support(v13::OFP_VERSION));
        BOOST_CHECK(sut.support(32));
        BOOST_CHECK(sut.support(48));
    }

BOOST_AUTO_TEST_SUITE_END() // support_test

BOOST_AUTO_TEST_SUITE(max_support_version_test)

    BOOST_AUTO_TEST_CASE(of13_support_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000010}};

        BOOST_CHECK_EQUAL(sut.max_support_version(), v13::OFP_VERSION);
    }

    BOOST_AUTO_TEST_CASE(of11_and_13_support_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000012}};

        BOOST_CHECK_EQUAL(sut.max_support_version(), v13::OFP_VERSION);
    }

    BOOST_AUTO_TEST_CASE(multi_bitmaps_test)
    {
        auto const sut = hello_elements::versionbitmap{{0x00000012, 0x00010001}};

        BOOST_CHECK_EQUAL(sut.max_support_version(), 48);
    }

BOOST_AUTO_TEST_SUITE_END() // max_support_version_test

BOOST_AUTO_TEST_SUITE_END() // hello_elements_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
