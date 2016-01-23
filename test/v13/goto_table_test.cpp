#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/instruction/goto_table.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(goto_table_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const table_id = std::uint8_t{0};

    auto const sut = instructions::goto_table{table_id};

    BOOST_CHECK_EQUAL(sut.type(), protocol::OFPIT_GOTO_TABLE);
    BOOST_CHECK_EQUAL(sut.length(), 8);
    BOOST_CHECK_EQUAL(sut.table_id(), table_id);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = instructions::goto_table{protocol::OFPTT_MAX};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.table_id(), sut.table_id());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = instructions::goto_table{protocol::OFPTT_MAX};

    auto const copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.table_id(), sut.table_id());
}

BOOST_AUTO_TEST_CASE(bod_table_id)
{
    BOOST_CHECK_THROW(instructions::goto_table{protocol::OFPTT_MAX + 1}, int);
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instructions::goto_table{1};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_instruction = instructions::goto_table::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_instruction.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_instruction.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_instruction.table_id(), sut.table_id());
}

BOOST_AUTO_TEST_SUITE_END() // goto_table_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
