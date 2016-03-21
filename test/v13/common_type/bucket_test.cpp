#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/bucket.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace actions = v13::actions;
namespace detail = v13::v13_detail;

using v13::protocol;

namespace {

struct bucket_fixture
{
    v13::bucket sut{
          0x1234, 0x01020304, 0xf1f2f3f4
        , v13::action_set{
              actions::decrement_nw_ttl{}
            , actions::set_udp_src{0xabcd}
            , actions::output{0x12345678}
          } // 8 + 16 + 16 = 40
    };
    std::vector<unsigned char> binary
        = "\x00\x38\x12\x34\x01\x02\x03\x04""\xf1\xf2\xf3\xf4\x00\x00\x00\x00"
          "\x00\x18\x00\x08\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x1e\x02"
          "\xab\xcd\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x10\x12\x34\x56\x78"
          "\xff\xff\x00\x00\x00\x00\x00\x00"
          ""_bin;
    std::vector<unsigned char> duplicated_action_binary
        = "\x00\x40\x12\x34\x01\x02\x03\x04""\xf1\xf2\xf3\xf4\x00\x00\x00\x00"
          "\x00\x18\x00\x08\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x1e\x02"
          "\xab\xcd\x00\x00\x00\x00\x00\x00""\x00\x00\x00\x10\x12\x34\x56\x78"
          "\xff\xff\x00\x00\x00\x00\x00\x00""\x00\x18\x00\x08\x00\x00\x00\x00"
          ""_bin;
};

}

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(bucket_test)

    BOOST_AUTO_TEST_CASE(construct_from_action_set_test)
    {
        auto const actions = v13::action_set{
              actions::decrement_nw_ttl{}
            , actions::output::to_controller()
        };

        auto const sut = v13::bucket{actions};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == 0);
        BOOST_TEST(sut.watch_port() == protocol::OFPP_ANY);
        BOOST_TEST(sut.watch_group() == protocol::OFPG_ANY);
        BOOST_TEST(sut.actions() == actions.to_list());
    }

    BOOST_AUTO_TEST_CASE(construct_from_weight_test)
    {
        auto const weight = std::uint16_t{3};
        auto const actions = v13::action_set{};

        auto const sut = v13::bucket{weight, actions};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == weight);
        BOOST_TEST(sut.watch_port() == protocol::OFPP_ANY);
        BOOST_TEST(sut.watch_group() == protocol::OFPG_ANY);
        BOOST_TEST(sut.actions() == actions.to_list());
    }

    BOOST_AUTO_TEST_CASE(construct_from_watch_port_and_group_set_test)
    {
        auto const watch_port = std::uint32_t{1};
        auto const watch_group = std::uint32_t{2};
        auto const actions = v13::action_set{
              actions::decrement_nw_ttl{}
        };

        auto const sut = v13::bucket{watch_port, watch_group, actions};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == 0);
        BOOST_TEST(sut.watch_port() == watch_port);
        BOOST_TEST(sut.watch_group() == watch_group);
        BOOST_TEST(sut.actions() == actions.to_list());
    }

    BOOST_AUTO_TEST_CASE(construct_from_all_arguments_test)
    {
        auto const weight = std::uint16_t{3};
        auto const watch_port = std::uint32_t{protocol::OFPP_MAX};
        auto const watch_group = std::uint32_t{0};
        auto const actions = v13::action_set{
              actions::decrement_nw_ttl{}
            , actions::set_udp_src{6653}
            , actions::output{1}
        };

        auto const sut = v13::bucket{weight, watch_port, watch_group, actions};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == weight);
        BOOST_TEST(sut.watch_port() == watch_port);
        BOOST_TEST(sut.watch_group() == watch_group);
        BOOST_TEST(sut.actions() == actions.to_list());
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, bucket_fixture)
    {
        auto const copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, bucket_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == sizeof(detail::ofp_bucket));
        BOOST_TEST(src.actions() == v13::action_list{});
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, bucket_fixture)
    {
        auto copy = v13::bucket{{}};

        copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, bucket_fixture)
    {
        auto src = sut;
        auto copy = v13::bucket{{}};

        copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == sizeof(detail::ofp_bucket));
        BOOST_TEST(src.actions() == v13::action_list{});
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut1 = v13::bucket{1, 2, 3, v13::action_set{
            actions::output{1}
        }};
        auto const sut2 = v13::bucket{v13::action_set{
            actions::output{1}
        }};
        auto const sut3 = v13::bucket{1, v13::action_set{
            actions::output{1}
        }};
        auto const sut4 = v13::bucket{2, 3, v13::action_set{
            actions::output{1}
        }};
        auto const sut5 = v13::bucket{1, 2, 3, v13::action_set{
            actions::output{2}
        }};

        BOOST_TEST((sut1 == sut1));
        BOOST_TEST((sut1 != sut2));
        BOOST_TEST((sut1 != sut3));
        BOOST_TEST((sut1 != sut4));
        BOOST_TEST((sut1 != sut5));
    }

    BOOST_AUTO_TEST_CASE(all_factory_test)
    {
        auto const actions = v13::action_set{actions::pop_vlan{}};

        auto const sut = v13::bucket::all(actions);

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == 0);
        BOOST_TEST(sut.watch_port() == protocol::OFPP_ANY);
        BOOST_TEST(sut.watch_group() == protocol::OFPG_ANY);
        BOOST_TEST((sut.actions() == actions.to_list()));
    }

    BOOST_AUTO_TEST_CASE(select_factory_test)
    {
        auto const weight = std::uint16_t{32};
        auto const actions = v13::action_set{actions::pop_vlan{}};

        auto const sut = v13::bucket::select(weight, actions);

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == weight);
        BOOST_TEST(sut.watch_port() == protocol::OFPP_ANY);
        BOOST_TEST(sut.watch_group() == protocol::OFPG_ANY);
        BOOST_TEST((sut.actions() == actions.to_list()));
    }

    BOOST_AUTO_TEST_CASE(indirect_factory_test)
    {
        auto const actions = v13::action_set{actions::pop_vlan{}};

        auto const sut = v13::bucket::indirect(actions);

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == 0);
        BOOST_TEST(sut.watch_port() == protocol::OFPP_ANY);
        BOOST_TEST(sut.watch_group() == protocol::OFPG_ANY);
        BOOST_TEST((sut.actions() == actions.to_list()));
    }

    BOOST_AUTO_TEST_CASE(failover_factory_test)
    {
        auto const actions = v13::action_set{actions::pop_vlan{}};

        auto const watch_port = std::uint32_t{1};
        auto const watch_group = std::uint32_t{protocol::OFPG_MAX};
        auto const sut
            = v13::bucket::failover(watch_port, watch_group, actions);

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_bucket) + actions.length());
        BOOST_TEST(sut.weight() == 0);
        BOOST_TEST(sut.watch_port() == watch_port);
        BOOST_TEST(sut.watch_group() == watch_group);
        BOOST_TEST((sut.actions() == actions.to_list()));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, bucket_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, bucket_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const bucket = v13::bucket::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((bucket == sut));
    }

    BOOST_FIXTURE_TEST_CASE(decode_duplicated_action_test, bucket_fixture)
    {
        auto it = duplicated_action_binary.begin();
        auto const it_end = duplicated_action_binary.end();

        auto const bucket = v13::bucket::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((bucket != sut));
        BOOST_CHECK_THROW(
                v13::bucket::validate(bucket), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // bucket_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
