#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/table_feature_property/actions.hpp>
#include <boost/test/unit_test.hpp>

#include <canard/network/protocol/openflow/v13/table_feature_property/id/action_id.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace table_feature_properties = v13::table_feature_properties;
namespace detail = v13::v13_detail;

using v13::protocol;

namespace {
struct write_actions_fixture
{
    table_feature_properties::write_actions sut{
          v13::action_id{protocol::OFPAT_OUTPUT}
        , v13::action_id{protocol::OFPAT_GROUP}
        , v13::action_id{protocol::OFPAT_SET_FIELD}
        , v13::action_id{protocol::OFPAT_POP_VLAN}
        , v13::action_id{protocol::OFPAT_PUSH_MPLS}
        , v13::action_id{protocol::OFPAT_SET_QUEUE}
        , v13::action_experimenter_id{0x12345678, {'A', 'B'}}
    };
    std::vector<unsigned char> binary
        = "\x00\x04\x00\x26\x00\x00\x00\x04""\x00\x16\x00\x04\x00\x19\x00\x04"
          "\x00\x12\x00\x04\x00\x13\x00\x04""\x00\x15\x00\x04\xff\xff\x00\x0a"
          "\x12\x34\x56\x78""A""B""\x00\x00"_bin;
};
}

BOOST_AUTO_TEST_SUITE(table_feature_property_test)

BOOST_AUTO_TEST_SUITE(write_actions_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::write_actions;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_WRITE_ACTIONS);
    }

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = table_feature_properties::write_actions{};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_actions));
        BOOST_TEST((sut.begin() == sut.end()));
        BOOST_TEST(sut.action_ids().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_id_test)
    {
        auto const output = v13::action_id{protocol::OFPAT_OUTPUT};

        auto const sut = table_feature_properties::write_actions{output};

        BOOST_TEST_REQUIRE(output.length() == 4);
        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_actions) + 4);
        BOOST_TEST((sut.begin() != sut.end()));
        BOOST_TEST(sut.action_ids().size() == 1);
        BOOST_TEST((*sut.begin() == output));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_ids_test)
    {
        auto const output = v13::action_id{protocol::OFPAT_OUTPUT};
        auto const set_field = v13::action_id{protocol::OFPAT_SET_FIELD};
        auto const experimenter
            = v13::action_experimenter_id{0x12345678, {'A', 'B'}};
        auto const push_vlan = v13::action_id{protocol::OFPAT_PUSH_VLAN};

        auto const sut = table_feature_properties::write_actions{
            output, set_field, experimenter, push_vlan
        };

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_actions)
                 + 4 * 3 + experimenter.length());
        BOOST_TEST((sut.begin() != sut.end()));
        BOOST_TEST_REQUIRE(sut.action_ids().size() == 4);
        auto it = sut.begin();
        BOOST_TEST((*it++ == output));
        BOOST_TEST((*it++ == set_field));
        BOOST_TEST((*it++ == experimenter));
        BOOST_TEST((*it++ == push_vlan));
        BOOST_TEST((it == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, write_actions_fixture)
    {
        auto const copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, write_actions_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length()
                == sizeof(detail::ofp_table_feature_prop_actions));
        BOOST_TEST((src.begin() == src.end()));
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, write_actions_fixture)
    {
        auto copy = table_feature_properties::write_actions{};

        copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, write_actions_fixture)
    {
        auto src = sut;
        auto copy = table_feature_properties::write_actions{};

        copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length()
                == sizeof(detail::ofp_table_feature_prop_actions));
        BOOST_TEST((src.begin() == src.end()));
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x12345678}
        };
        auto const diff_order = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x12345678}
            , v13::action_id{protocol::OFPAT_OUTPUT}
        };
        auto const diff_value = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x87654321}
        };
        auto const diff_type = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_GROUP}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x12345678}
        };
        auto const diff_num1 = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_experimenter_id{0x12345678}
        };
        auto const diff_num2 = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
        };
        auto const diff_num3 = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x12345678}
            , v13::action_id{protocol::OFPAT_SET_QUEUE}
        };
        auto const diff_num4 = table_feature_properties::write_actions{
              v13::action_id{protocol::OFPAT_OUTPUT}
            , v13::action_id{protocol::OFPAT_SET_FIELD}
            , v13::action_experimenter_id{0x12345678}
            , v13::action_experimenter_id{0x87654321}
        };
        auto const empty = table_feature_properties::write_actions{};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut != diff_order));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != diff_type));
        BOOST_TEST((sut != diff_num1));
        BOOST_TEST((sut != diff_num2));
        BOOST_TEST((sut != diff_num3));
        BOOST_TEST((sut != diff_num4));
        BOOST_TEST((sut != empty));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, write_actions_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == detail::exact_length(sut.length()));
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, write_actions_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const write_actions
            = table_feature_properties::write_actions::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((write_actions == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // write_actions_test

BOOST_AUTO_TEST_SUITE(write_actions_miss_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::write_actions_miss;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_WRITE_ACTIONS_MISS);
    }

BOOST_AUTO_TEST_SUITE_END() // write_actions_test

BOOST_AUTO_TEST_SUITE(apply_actions_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::apply_actions;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_APPLY_ACTIONS);
    }

BOOST_AUTO_TEST_SUITE_END() // write_actions_test

BOOST_AUTO_TEST_SUITE(apply_actions_miss_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::apply_actions_miss;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_APPLY_ACTIONS_MISS);
    }

BOOST_AUTO_TEST_SUITE_END() // write_actions_test

BOOST_AUTO_TEST_SUITE_END() // table_feature_property_test
