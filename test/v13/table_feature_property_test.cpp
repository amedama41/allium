#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <canard/network/protocol/openflow/v13/message/multipart_message/table_feature_property.hpp>
#include <iterator>
#include <boost/endian/conversion.hpp>
#include <canard/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(table_feature_property_test)

BOOST_AUTO_TEST_SUITE(prop_instructions_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = table_feature_properties::prop_instructions{};

        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPTFPT_INSTRUCTIONS);
        BOOST_CHECK_EQUAL(sut.length(), 4);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 0);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = table_feature_properties::prop_instructions{
              instruction_id{protocol::OFPIT_GOTO_TABLE}
            , instruction_id{protocol::OFPIT_WRITE_METADATA}
            , instruction_id{protocol::OFPIT_WRITE_ACTIONS}
            , instruction_id{protocol::OFPIT_APPLY_ACTIONS}
            , instruction_id{protocol::OFPIT_CLEAR_ACTIONS}
            , instruction_id{protocol::OFPIT_METER}
            , instruction_experimenter_id{32}
        };

        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPTFPT_INSTRUCTIONS);
        BOOST_CHECK_EQUAL(sut.length(), 4 + 4 * 6 + 8);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 7);
        auto it = sut.begin();
        BOOST_CHECK_EQUAL(it->type(), protocol::OFPIT_GOTO_TABLE);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_WRITE_METADATA);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_WRITE_ACTIONS);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_APPLY_ACTIONS);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_CLEAR_ACTIONS);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_METER);
        BOOST_CHECK_EQUAL((++it)->type(), protocol::OFPIT_EXPERIMENTER);
    }

    BOOST_AUTO_TEST_CASE(copy_constructor_test)
    {
        auto const sut = table_feature_properties::prop_instructions{
              instruction_id{protocol::OFPIT_GOTO_TABLE}
            , instruction_id{protocol::OFPIT_WRITE_ACTIONS}
            , instruction_id{protocol::OFPIT_CLEAR_ACTIONS}
            , instruction_experimenter_id{0, {'A', 'B', 'C', 'D'}}
        };

        auto const copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(std::distance(copy.begin(), copy.end()), std::distance(sut.begin(), sut.end()));
        auto copy_it = copy.begin(), it = sut.begin();
        BOOST_CHECK_EQUAL(copy_it->type(), it->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
    }

    BOOST_AUTO_TEST_CASE(move_constructor_test)
    {
        auto sut = table_feature_properties::prop_instructions{
              instruction_id{protocol::OFPIT_GOTO_TABLE}
        };

        auto const copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), 4 + 4);
        BOOST_CHECK_EQUAL(sut.length(), 4 + 0);
        BOOST_CHECK_EQUAL(std::distance(copy.begin(), copy.end()), 1);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 0);
        auto copy_it = copy.begin();
        BOOST_CHECK_EQUAL(copy_it->type(), protocol::OFPIT_GOTO_TABLE);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE(assignment_test)

    BOOST_AUTO_TEST_CASE(copy_assign_test)
    {
        auto const sut = table_feature_properties::prop_instructions{
              instruction_id{protocol::OFPIT_GOTO_TABLE}
            , instruction_id{protocol::OFPIT_WRITE_ACTIONS}
            , instruction_id{protocol::OFPIT_CLEAR_ACTIONS}
            , instruction_experimenter_id{0, {'A', 'B', 'C', 'D'}}
        };
        auto copy = table_feature_properties::prop_instructions{};

        copy = sut;

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
        BOOST_CHECK_EQUAL(std::distance(copy.begin(), copy.end()), std::distance(sut.begin(), sut.end()));
        auto copy_it = copy.begin(), it = sut.begin();
        BOOST_CHECK_EQUAL(copy_it->type(), it->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
    }

    BOOST_AUTO_TEST_CASE(move_assign_test)
    {
        auto sut = table_feature_properties::prop_instructions{
            instruction_experimenter_id{0xffffffff, {'A', 'B', 'C', 'D'}}
        };
        auto copy = table_feature_properties::prop_instructions{};

        copy = std::move(sut);

        BOOST_CHECK_EQUAL(copy.type(), sut.type());
        BOOST_CHECK_EQUAL(copy.length(), 4 + 12);
        BOOST_CHECK_EQUAL(sut.length(), 4 + 0);
        BOOST_CHECK_EQUAL(std::distance(copy.begin(), copy.end()), 1);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 0);
        auto copy_it = copy.begin();
        BOOST_CHECK_EQUAL(copy_it->type(), protocol::OFPIT_EXPERIMENTER);
        BOOST_CHECK_EQUAL(copy_it->length(), 12);
    }

BOOST_AUTO_TEST_SUITE_END() // assignment_test

struct encode_decode_fixture {
    table_feature_properties::prop_instructions sut{
          instruction_id{protocol::OFPIT_GOTO_TABLE}
        , instruction_id{protocol::OFPIT_WRITE_METADATA}
        , instruction_id{protocol::OFPIT_WRITE_ACTIONS}
        , instruction_id{protocol::OFPIT_APPLY_ACTIONS}
        , instruction_id{protocol::OFPIT_CLEAR_ACTIONS}
        , instruction_id{protocol::OFPIT_METER}
        , instruction_experimenter_id{0x12345678, {'A', 'B'}}
    };
    std::vector<std::uint8_t> buffer{};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), v13_detail::exact_length(sut.length()));
        auto ptr = buffer.data();
        auto type = std::uint16_t{};
        auto length = std::uint16_t{};
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPTFPT_INSTRUCTIONS);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4 + (4 * 6) + (4 + 4 + 2));
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_GOTO_TABLE);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_WRITE_METADATA);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_WRITE_ACTIONS);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_APPLY_ACTIONS);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_CLEAR_ACTIONS);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_METER);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4);
        std::memcpy(&type, ptr, sizeof(type));
        ptr += sizeof(type);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(type), protocol::OFPIT_EXPERIMENTER);
        std::memcpy(&length, ptr, sizeof(length));
        ptr += sizeof(length);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(length), 4 + 4 + 2);
        auto experimenter = std::uint32_t{};
        std::memcpy(&experimenter, ptr, sizeof(experimenter));
        ptr += sizeof(experimenter);
        BOOST_CHECK_EQUAL(boost::endian::big_to_native(experimenter), 0x12345678);
        auto const expected = "AB\0\0";
        BOOST_CHECK_EQUAL_COLLECTIONS(ptr, ptr + 4, expected, expected + 4);
    }

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        sut.encode(buffer);
        auto first = buffer.begin();

        auto const copy_prop = table_feature_properties::prop_instructions::decode(first, buffer.end());

        BOOST_REQUIRE(first == buffer.end());
        BOOST_CHECK_EQUAL(copy_prop.type(), sut.type());
        BOOST_CHECK_EQUAL(copy_prop.length(), sut.length());
        BOOST_REQUIRE_EQUAL(std::distance(copy_prop.begin(), copy_prop.end()), std::distance(sut.begin(), sut.end()));
        auto copy_it = copy_prop.begin(), it = sut.begin();
        BOOST_CHECK_EQUAL(copy_it->type(), it->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
        BOOST_CHECK_EQUAL((++copy_it)->type(), (++it)->type());
        BOOST_CHECK_EQUAL(copy_it->length(), it->length());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // prop_instructions_test


BOOST_AUTO_TEST_SUITE(prop_next_tables_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = table_feature_properties::prop_next_tables{};

        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPTFPT_NEXT_TABLES);
        BOOST_CHECK_EQUAL(sut.length(), 4);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 0);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto const sut = table_feature_properties::prop_next_tables{
            1, 2, 3, 4, 5, 6, 7, 255
        };

        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPTFPT_NEXT_TABLES);
        BOOST_CHECK_EQUAL(sut.length(), 4 + 8);
        BOOST_CHECK_EQUAL(std::distance(sut.begin(), sut.end()), 8);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_SUITE_END() // prop_next_tables_test

BOOST_AUTO_TEST_SUITE_END() // table_feature_property_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
