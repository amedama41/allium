#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/push_pbb.hpp>
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

struct push_pbb_fixture
{
    actions::push_pbb sut{};
    std::vector<std::uint8_t> binary = "\x00\x1a\x00\x08\x88\xe7\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(action_test)
BOOST_AUTO_TEST_SUITE(push_pbb_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::push_pbb;

        BOOST_TEST(sut::type() == protocol::OFPAT_PUSH_PBB);
        BOOST_TEST(sut::length() == sizeof(v13_detail::ofp_action_push));
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = actions::push_pbb{};

        BOOST_TEST(sut.ethertype() == 0x88e7);
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        auto const sut = actions::push_pbb::create();

        BOOST_TEST(sut.ethertype() == 0x88e7);
    }

    BOOST_AUTO_TEST_CASE(validate_failure_test)
    {
        auto const binary = "\x00\x1a\x00\x08\x88\xe8\x00\x00"_bin;
        auto it = binary.begin();
        auto const sut = actions::push_pbb::decode(it, binary.end());

        BOOST_CHECK_THROW(actions::push_pbb::validate(sut), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = actions::push_pbb{};

        BOOST_TEST((sut == sut));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, push_pbb_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, push_pbb_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const action = actions::push_pbb::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((action == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // push_pbb_test
BOOST_AUTO_TEST_SUITE_END() // action_test
