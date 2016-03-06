#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/action/pop_mpls.hpp>
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

struct pop_mpls_fixture
{
    actions::pop_mpls sut{0x0800};
    std::vector<std::uint8_t> binary = "\x00\x14\x00\x08\x08\x00\x00\x00"_bin;
};

}

BOOST_AUTO_TEST_SUITE(action_test)
BOOST_AUTO_TEST_SUITE(pop_mpls_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = actions::pop_mpls;

        BOOST_TEST(sut::type() == protocol::OFPAT_POP_MPLS);
        BOOST_TEST(sut::length() == sizeof(v13_detail::ofp_action_pop_mpls));
    }

    BOOST_AUTO_TEST_CASE(construct_test)
    {
        auto const ethertype = std::uint16_t{0x8847};

        auto const sut = actions::pop_mpls{ethertype};

        BOOST_TEST(sut.ethertype() == ethertype);
    }

    BOOST_AUTO_TEST_CASE(ipv4_test)
    {
        auto const sut = actions::pop_mpls::ipv4();

        BOOST_TEST(sut.ethertype() == 0x0800);
    }

    BOOST_AUTO_TEST_CASE(unicast_test)
    {
        auto const sut = actions::pop_mpls::unicast();

        BOOST_TEST(sut.ethertype() == 0x8847);
    }

    BOOST_AUTO_TEST_CASE(multicast_test)
    {
        auto const sut = actions::pop_mpls::multicast();

        BOOST_TEST(sut.ethertype() == 0x8848);
    }

    BOOST_DATA_TEST_CASE(
            create_test
            , bdata::make(std::vector<std::uint16_t>{0x0800, 0x8847, 0x8848})
            , ethertype)
    {
        auto const sut = actions::pop_mpls::create(ethertype);

        BOOST_TEST(sut.ethertype() == ethertype);
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = actions::pop_mpls{0x0800};
        auto const same_value = actions::pop_mpls{0x0800};
        auto const diff_value = actions::pop_mpls{0x86dd};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut == same_value));
        BOOST_TEST((sut != diff_value));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, pop_mpls_fixture)
    {
        auto buffer = std::vector<std::uint8_t>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, pop_mpls_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const action = actions::pop_mpls::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((action == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // pop_mpls_test
BOOST_AUTO_TEST_SUITE_END() // action_test
