#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction/goto_table.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace instructions = v13::instructions;
namespace detail = v13::v13_detail;

namespace bdata = boost::unit_test::data;

namespace {

struct goto_table_fixture
{
    instructions::goto_table sut{0x12};
    std::vector<unsigned char> binary = "\x00\x01\x00\x08\x12\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(instruction_test)
BOOST_AUTO_TEST_SUITE(goto_table_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = instructions::goto_table;

        BOOST_TEST(sut::type() == v13::protocol::OFPIT_GOTO_TABLE);
        BOOST_TEST(sut::length() == sizeof(detail::ofp_instruction_goto_table));
    }

    BOOST_AUTO_TEST_CASE(construct_from_table_id_test)
    {
        auto const table_id = std::uint8_t{1};

        auto const sut = instructions::goto_table{table_id};

        BOOST_TEST(sut.table_id() == table_id);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint8_t>{
                0x00, v13::protocol::OFPTT_MAX
              })
            , table_id)
    {
        BOOST_CHECK_NO_THROW(instructions::goto_table::create(table_id));
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(v13::protocol::OFPTT_ALL)
            , table_id)
    {
        BOOST_CHECK_THROW(
                  instructions::goto_table::create(table_id)
                , std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = instructions::goto_table{0x12};
        auto const sut2 = instructions::goto_table{0x13};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 != sut2));
        BOOST_TEST((sut2 == sut2));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, goto_table_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, goto_table_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const goto_table = instructions::goto_table::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((goto_table == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // goto_table_test
BOOST_AUTO_TEST_SUITE_END() // instruction_test
