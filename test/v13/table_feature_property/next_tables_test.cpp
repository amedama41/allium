#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/table_feature_property/next_tables.hpp>
#include <boost/test/unit_test.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace table_feature_properties = v13::table_feature_properties;
namespace detail = v13::v13_detail;

using v13::protocol;

namespace {
struct next_tables_fixture
{
    table_feature_properties::next_tables sut{
        0x01, 0x02, 0x03, 0x05, 0x07, 0x0b
    };
    std::vector<unsigned char> binary
        = "\x00\x02\x00\x0a\x01\x02\x03\x05""\x07\x0b\x00\x00\x00\x00\x00\x00"
          ""_bin;
};
}

BOOST_AUTO_TEST_SUITE(table_feature_property_test)

BOOST_AUTO_TEST_SUITE(next_tables_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::next_tables;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_NEXT_TABLES);
    }

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = table_feature_properties::next_tables{};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_next_tables));
        BOOST_TEST((sut.begin() == sut.end()));
        BOOST_TEST(sut.next_table_ids().empty());
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_id_test)
    {
        auto const table_id = std::uint8_t{0x12};

        auto const sut = table_feature_properties::next_tables{table_id};

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_next_tables) + 1);
        BOOST_TEST(sut.next_table_ids().size() == 1);
        auto it = sut.begin();
        BOOST_TEST_REQUIRE((it != sut.end()));
        BOOST_TEST((*it++ == table_id));
        BOOST_TEST((it == sut.end()));
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_ids_test)
    {
        auto const table_id1 = std::uint8_t{0x01};
        auto const table_id2 = std::uint8_t{0x02};
        auto const table_id3 = std::uint8_t{0x05};

        auto const sut = table_feature_properties::next_tables{
            table_id1, table_id2, table_id3
        };

        BOOST_TEST(sut.length()
                == sizeof(detail::ofp_table_feature_prop_next_tables) + 1 * 3);
        BOOST_TEST(sut.next_table_ids().size() == 3);
        auto it = sut.begin();
        BOOST_TEST_REQUIRE((it != sut.end()));
        BOOST_TEST((*it++ == table_id1));
        BOOST_TEST((*it++ == table_id2));
        BOOST_TEST((*it++ == table_id3));
        BOOST_TEST((it == sut.end()));
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, next_tables_fixture)
    {
        auto const copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, next_tables_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length()
                == sizeof(detail::ofp_table_feature_prop_next_tables));
        BOOST_TEST(src.next_table_ids().empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, next_tables_fixture)
    {
        auto copy = table_feature_properties::next_tables{};

        copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, next_tables_fixture)
    {
        auto src = sut;
        auto copy = table_feature_properties::next_tables{};

        copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length()
                == sizeof(detail::ofp_table_feature_prop_next_tables));
        BOOST_TEST(src.next_table_ids().empty());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut
            = table_feature_properties::next_tables{0x01, 0x02, 0x03};
        auto const diff_order
            = table_feature_properties::next_tables{0x02, 0x03, 0x01};
        auto const diff_value
            = table_feature_properties::next_tables{0x01, 0x04, 0x03};
        auto const diff_num1
            = table_feature_properties::next_tables{0x01, 0x03};
        auto const diff_num2
            = table_feature_properties::next_tables{0x01, 0x02};
        auto const diff_num3
            = table_feature_properties::next_tables{0x01, 0x02, 0x03, 0x04};
        auto const empty = table_feature_properties::next_tables{};

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut != diff_order));
        BOOST_TEST((sut != diff_value));
        BOOST_TEST((sut != diff_num1));
        BOOST_TEST((sut != diff_num2));
        BOOST_TEST((sut != diff_num3));
        BOOST_TEST((sut != empty));
    }

    BOOST_FIXTURE_TEST_CASE(encode_test, next_tables_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == detail::exact_length(sut.length()));
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, next_tables_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const next_tables
            = table_feature_properties::next_tables::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((next_tables == sut));
    }

BOOST_AUTO_TEST_SUITE_END() // next_tables_test

BOOST_AUTO_TEST_SUITE(next_tables_miss_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = table_feature_properties::next_tables_miss;

        BOOST_TEST(sut::type() == protocol::OFPTFPT_NEXT_TABLES_MISS);
    }

BOOST_AUTO_TEST_SUITE_END() // next_tables_test

BOOST_AUTO_TEST_SUITE_END() // table_feature_property_test
