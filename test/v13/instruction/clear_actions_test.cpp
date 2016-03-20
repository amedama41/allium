#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction/clear_actions.hpp>
#include <boost/test/unit_test.hpp>

#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace instructions = v13::instructions;
namespace detail = v13::v13_detail;

namespace {

struct clear_actions_fixture
{
    instructions::clear_actions sut{};
    std::vector<unsigned char> binary = "\x00\x05\x00\x08\x00\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(instruction_test)
BOOST_AUTO_TEST_SUITE(clear_actions_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = instructions::clear_actions;

        BOOST_TEST(sut::type() == v13::protocol::OFPIT_CLEAR_ACTIONS);
        BOOST_TEST(sut::length() == sizeof(detail::ofp_instruction_actions));
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const sut = instructions::clear_actions{};

        BOOST_TEST(sut.length() == 8);
    }

    BOOST_AUTO_TEST_CASE(create_test)
    {
        BOOST_CHECK_NO_THROW(instructions::clear_actions::create());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = instructions::clear_actions{};
        auto const sut2 = instructions::clear_actions{};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, clear_actions_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, clear_actions_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const clear_actions
            = instructions::clear_actions::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((clear_actions == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // clear_actions_test
BOOST_AUTO_TEST_SUITE_END() // instruction_test
