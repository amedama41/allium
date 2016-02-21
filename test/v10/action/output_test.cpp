#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v10/action/output.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace of = canard::network::openflow;
namespace v10 = of::v10;
namespace detail = v10::v10_detail;
namespace actions = v10::actions;

namespace {

auto operator""_bin(char const* const str, std::size_t const size)
    -> std::vector<std::uint8_t>
{
    return std::vector<std::uint8_t>(str, str + size);
}

struct output_fixture
{
    actions::output sut = actions::output::to_controller(0x1234);
    std::vector<std::uint8_t> binary
        = "\x00\x00\x00\x08\xff\xfd\x12\x34"_bin;
};

}

BOOST_AUTO_TEST_SUITE(actions_test)
BOOST_AUTO_TEST_SUITE(output_test)

    BOOST_AUTO_TEST_CASE(construct_from_port_test)
    {
        auto const port_no = std::uint16_t{1};

        auto const sut = actions::output{port_no};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_OUTPUT);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_output));
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.max_length() == std::numeric_limits<std::uint16_t>::max());
    }

    BOOST_AUTO_TEST_CASE(construct_from_port_and_max_len_test)
    {
        auto const port_no = std::uint16_t{v10::protocol::OFPP_IN_PORT};
        auto const max_len = std::uint16_t{32};

        auto const sut = actions::output{port_no, max_len};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_OUTPUT);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_output));
        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.max_length() == max_len);
    }

    BOOST_AUTO_TEST_CASE(to_controller_test)
    {
        auto const max_len = std::uint16_t{1};

        auto const sut = actions::output::to_controller(max_len);

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_OUTPUT);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_output));
        BOOST_TEST(sut.port_no() == v10::protocol::OFPP_CONTROLLER);
        BOOST_TEST(sut.max_length() == max_len);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , boost::unit_test::data::make(std::vector<std::uint16_t>{
                1, v10::protocol::OFPP_MAX, v10::protocol::OFPP_LOCAL
              })
            * boost::unit_test::data::make(std::vector<std::uint16_t>{
                0, std::numeric_limits<std::uint16_t>::max()
              })
            , port_no, max_len)
    {
        auto const sut = actions::output::create(port_no, max_len);

        BOOST_TEST(sut.port_no() == port_no);
        BOOST_TEST(sut.max_length() == max_len);
    }

    BOOST_DATA_TEST_CASE(
            create_failure_test
          , boost::unit_test::data::make(std::vector<std::uint16_t>{
              0, v10::protocol::OFPP_NONE
            })
          , port_no)
    {
        BOOST_CHECK_THROW(actions::output::create(port_no), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(normal_port_equality_test)
    {
        auto const sut1 = actions::output{v10::protocol::OFPP_MAX};
        auto const sut2 = actions::output{v10::protocol::OFPP_MAX};
        auto const sut3 = actions::output{v10::protocol::OFPP_MAX, 0};
        auto const sut4 = actions::output{1};
        auto const sut5 = actions::output{v10::protocol::OFPP_CONTROLLER};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 == sut3));
        BOOST_TEST((sut1 != sut4));
        BOOST_TEST((sut1 != sut5));
    }

    BOOST_AUTO_TEST_CASE(to_controller_equality_test)
    {
        auto const sut1 = actions::output::to_controller(0x1234);
        auto const sut2 = actions::output::to_controller(0x1234);
        auto const sut3 = actions::output::to_controller(0x1235);
        auto const sut4 = actions::output{1};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
        BOOST_TEST((sut1 != sut4));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, output_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, output_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const output = actions::output::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((output == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // output_test
BOOST_AUTO_TEST_SUITE_END() // actions_test
