#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/any_instruction.hpp>
#include <boost/test/unit_test.hpp>

#include <vector>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;
namespace instructions = v13::instructions;

namespace {

struct any_instruction_fixture
{
    v13::any_instruction sut{
        instructions::write_metadata{0x0102030405060708, 0x1f2f3f4f5f6f7f8f}
    };
    std::vector<unsigned char> binary
        = "\x00\x02\x00\x18\x00\x00\x00\x00""\x01\x02\x03\x04\x05\x06\x07\x08"
          "\x1f\x2f\x3f\x4f\x5f\x6f\x7f\x8f"_bin;
};

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(any_instruction_test)

    BOOST_AUTO_TEST_CASE(construct_from_goto_table_test)
    {
        auto const goto_table = instructions::goto_table{1};

        auto const sut = v13::any_instruction{goto_table};

        BOOST_TEST(sut.type() == goto_table.type());
        BOOST_TEST(sut.length() == goto_table.length());
        BOOST_TEST((sut == goto_table));
        BOOST_TEST(!v13::any_cast<instructions::clear_actions>(&sut));
        BOOST_TEST_REQUIRE(v13::any_cast<instructions::goto_table>(&sut));
        BOOST_TEST((v13::any_cast<instructions::goto_table>(sut)
                    == goto_table));
    }

    BOOST_AUTO_TEST_CASE(construct_from_apply_actions_test)
    {
        auto const apply_actions = instructions::apply_actions{
            actions::set_tcp_dst{6653}, actions::output{1}
        };

        auto const sut = v13::any_instruction{apply_actions};

        BOOST_TEST(sut.type() == apply_actions.type());
        BOOST_TEST(sut.length() == apply_actions.length());
        BOOST_TEST((sut == apply_actions));
        BOOST_TEST(!v13::any_cast<instructions::clear_actions>(&sut));
        BOOST_TEST_REQUIRE(v13::any_cast<instructions::apply_actions>(&sut));
        BOOST_TEST((v13::any_cast<instructions::apply_actions>(sut)
                    == apply_actions));
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = v13::any_instruction{instructions::goto_table{1}};
        auto const sut2 = v13::any_instruction{instructions::goto_table{2}};
        auto const sut3 = v13::any_instruction{instructions::clear_actions{}};
        auto const goto_table1 = instructions::goto_table{1};
        auto const goto_table2 = instructions::goto_table{2};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 != sut2));
        BOOST_TEST((sut1 != sut3));
        BOOST_TEST((sut1 == goto_table1));
        BOOST_TEST((sut1 != goto_table2));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, any_instruction_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, any_instruction_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const instruction = v13::any_instruction::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((instruction == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // any_instruction_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
