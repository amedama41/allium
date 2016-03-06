#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/copy_ttl_in.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;
namespace v13_detail = v13::v13_detail;

using protocol = v13::protocol;

namespace bdata = boost::unit_test::data;

namespace {

struct copy_ttl_in_fixture
{
    actions::copy_ttl_in sut{};
    std::vector<std::uint8_t> binary = "\x00\x0c\x00\x08\x00\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(action_test)
BOOST_AUTO_TEST_SUITE(copy_ttl_in_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::copy_ttl_in;

        BOOST_TEST(sut::type() == protocol::OFPAT_COPY_TTL_IN);
        BOOST_TEST(sut::length() == sizeof(v13_detail::ofp_action_header));
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = actions::copy_ttl_in{};

        BOOST_TEST(((void)sut, true));
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const sut = actions::copy_ttl_in::create();

        BOOST_TEST(((void)sut, true));
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = actions::copy_ttl_in{};

        BOOST_TEST((sut == sut));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, copy_ttl_in_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, copy_ttl_in_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const action = actions::copy_ttl_in::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((action == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // copy_ttl_in_test
BOOST_AUTO_TEST_SUITE_END() // action_test
