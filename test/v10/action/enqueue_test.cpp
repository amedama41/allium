#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v10/action/enqueue.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <cstddef>
#include <cstdint>
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

struct enqueue_fixture
{
    actions::enqueue sut{0x12345678, v10::protocol::OFPP_MAX};
    std::vector<std::uint8_t> binary
        = "\x00\x0b\x00\x10\xff\x00\x00\x00""\x00\x00\x00\x00\x12\x34\x56\x78"
          ""_bin;
};

}

BOOST_AUTO_TEST_SUITE(actions_test)
BOOST_AUTO_TEST_SUITE(enqueue_test)

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const queue_id = std::uint32_t{0};
        auto const port_no = std::uint16_t{1};

        auto const sut = actions::enqueue{queue_id, port_no};

        BOOST_TEST(sut.type() == v10::protocol::OFPAT_ENQUEUE);
        BOOST_TEST(sut.length() == sizeof(detail::ofp_action_enqueue));
        BOOST_TEST(sut.queue_id() == queue_id);
        BOOST_TEST(sut.port_no() == port_no);
    }

    BOOST_DATA_TEST_CASE(
              create_success_test
            , boost::unit_test::data::make(std::vector<std::uint32_t>{
                0, 0xfffffffe
              })
            * boost::unit_test::data::make(std::vector<std::uint16_t>{
                1, v10::protocol::OFPP_MAX, v10::protocol::OFPP_IN_PORT
              })
            , queue_id, port_no)
    {
        auto const sut = actions::enqueue::create(queue_id, port_no);

        BOOST_TEST(sut.queue_id() == queue_id);
        BOOST_TEST(sut.port_no() == port_no);
    }

    BOOST_AUTO_TEST_CASE(create_failure_test)
    {
        BOOST_CHECK_THROW(actions::enqueue::create(0, 0), std::runtime_error);
        BOOST_CHECK_THROW(
                  actions::enqueue::create(0, v10::protocol::OFPP_MAX + 1)
                , std::runtime_error);
        BOOST_CHECK_THROW(
                  actions::enqueue::create(v10::protocol::OFPQ_ALL, 1)
                , std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = actions::enqueue{1, v10::protocol::OFPP_MAX};
        auto const sut2 = actions::enqueue{1, v10::protocol::OFPP_MAX};
        auto const sut3 = actions::enqueue{2, v10::protocol::OFPP_MAX};
        auto const sut4 = actions::enqueue{1, 1};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 == sut2));
        BOOST_TEST((sut1 != sut3));
        BOOST_TEST((sut1 != sut4));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, enqueue_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, enqueue_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const enqueue = actions::enqueue::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((enqueue == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // enqueue_test
BOOST_AUTO_TEST_SUITE_END() // actions_test

