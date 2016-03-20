#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction/meter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdint>
#include <vector>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace instructions = v13::instructions;
namespace detail = v13::v13_detail;

namespace bdata = boost::unit_test::data;

namespace {

struct meter_fixture
{
    instructions::meter sut{0x12345678};
    std::vector<unsigned char> binary = "\x00\x06\x00\x08\x12\x34\x56\x78"_bin;
};

}

BOOST_AUTO_TEST_SUITE(instruction_test)
BOOST_AUTO_TEST_SUITE(meter_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = instructions::meter;

        BOOST_TEST(sut::type() == v13::protocol::OFPIT_METER);
        BOOST_TEST(sut::length() == sizeof(detail::ofp_instruction_meter));
    }

    BOOST_AUTO_TEST_CASE(construct_from_meter_id_test)
    {
        auto const meter_id = std::uint32_t{0x12345678};

        auto const sut = instructions::meter{meter_id};

        BOOST_TEST(sut.meter_id() == meter_id);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , bdata::make(std::vector<std::uint32_t>{
                  1, v13::protocol::OFPM_MAX
              })
            , meter_id)
    {
        BOOST_CHECK_NO_THROW(instructions::meter::create(meter_id));
    }

    BOOST_DATA_TEST_CASE(
              create_failure_test
            , bdata::make(std::vector<std::uint32_t>{
                  0
                , v13::protocol::OFPM_SLOWPATH
                , v13::protocol::OFPM_CONTROLLER
                , v13::protocol::OFPM_ALL
              })
            , meter_id)
    {
        BOOST_CHECK_THROW(
                  instructions::meter::create(meter_id)
                , std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = instructions::meter{1};
        auto const sut2 = instructions::meter{2};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 != sut2));
        BOOST_TEST((sut2 == sut2));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, meter_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, meter_fixture)
    {
        auto it = binary.begin();
        auto it_end = binary.end();

        auto const meter = instructions::meter::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((meter == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // meter_test
BOOST_AUTO_TEST_SUITE_END() // instruction_test
