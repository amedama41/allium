#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/endian/conversion.hpp>
#include <canard/unit_test.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/instructions.hpp>

#include "../../test_utility.hpp"

namespace of = canard::network::openflow;
namespace v13 = of::v13;
namespace oxm_match = v13::oxm_match;
namespace actions = v13::actions;
namespace instructions = v13::instructions;
namespace detail = v13::v13_detail;

using v13::protocol;

namespace {

struct instruction_set_fixture
{
    v13::instruction_set sut{
          instructions::write_actions{
              actions::set_ipv4_src{"192.168.10.1"_ipv4}
            , actions::set_ipv4_dst{"172.16.1.2"_ipv4}
          } // 8 + 16 + 16 = 40
        , instructions::clear_actions{} // 8
        , instructions::apply_actions{
              actions::set_eth_type{0x0800}
            , actions::set_eth_src{"\x01\x02\x03\x04\x05\x06"_mac}
          } // 8 + 16 + 16 = 40
        , instructions::goto_table{0x12} // 8
    }; // 40 + 8 + 40 + 8 = 96
    std::vector<unsigned char> binary = 
        "\x00\x04\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x0a\x02"
        "\x08\x00\x00\x00\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x08\x06"
        "\x01\x02\x03\x04\x05\x06\x00\x00"

        "\x00\x05\x00\x08\x00\x00\x00\x00"

        "\x00\x03\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
        "\xc0\xa8\x0a\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x18\x04"
        "\xac\x10\x01\x02\x00\x00\x00\x00"

        "\x00\x01\x00\x08\x12\x00\x00\x00"
    ""_bin;
    std::vector<unsigned char> diff_order_binary = 
        "\x00\x03\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
        "\xc0\xa8\x0a\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x18\x04"
        "\xac\x10\x01\x02\x00\x00\x00\x00"

        "\x00\x05\x00\x08\x00\x00\x00\x00"

        "\x00\x04\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x0a\x02"
        "\x08\x00\x00\x00\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x08\x06"
        "\x01\x02\x03\x04\x05\x06\x00\x00"

        "\x00\x01\x00\x08\x12\x00\x00\x00"
    ""_bin;
    std::vector<unsigned char> duplicated_binary = 
        "\x00\x03\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
        "\xc0\xa8\x0a\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x18\x04"
        "\xac\x10\x01\x02\x00\x00\x00\x00"

        "\x00\x05\x00\x08\x00\x00\x00\x00"

        "\x00\x03\x00\x28\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x16\x04"
        "\xc0\xa8\x0a\x01\x00\x00\x00\x00""\x00\x19\x00\x10\x80\x00\x18\x04"
        "\xac\x10\x01\x02\x00\x00\x00\x00"

        "\x00\x05\x00\x08\x00\x00\x00\x00"
    ""_bin;
};

};

BOOST_AUTO_TEST_SUITE(common_type_test)
BOOST_AUTO_TEST_SUITE(instruction_set_test)

    BOOST_AUTO_TEST_CASE(type_definition_test)
    {
        using sut = v13::instruction_set;

        BOOST_TEST((std::is_same<sut::key_type, std::uint64_t>::value));
        BOOST_TEST((std::is_same<
                      sut::value_type, v13::any_instruction
                    >::value));
        BOOST_TEST((std::is_same<
                      sut::reference, v13::any_instruction const&
                    >::value));
        BOOST_TEST((std::is_same<
                      sut::const_reference, v13::any_instruction const&
                    >::value));
        BOOST_TEST((std::is_same<
                      std::iterator_traits<sut::iterator>::reference
                    , v13::any_instruction const&
                    >::value));
        BOOST_TEST((std::is_same<
                      std::iterator_traits<sut::const_iterator>::reference
                    , v13::any_instruction const&
                    >::value));
    }

    BOOST_AUTO_TEST_CASE(default_construct_test)
    {
        auto const sut = v13::instruction_set{};

        BOOST_TEST(sut.length() == 0);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));

        auto const goto_table_order
            = v13::instruction_order<instructions::goto_table>::value;
        BOOST_TEST(!(sut.find<instructions::write_actions>()));
        BOOST_TEST((sut.find(goto_table_order) == sut.end()));
        BOOST_CHECK_THROW(
                  sut.at<instructions::apply_actions>(), std::out_of_range);
        BOOST_CHECK_THROW(sut.at(goto_table_order), std::out_of_range);
    }

    BOOST_AUTO_TEST_CASE(construct_from_single_instruction_test)
    {
        auto const write_actions = instructions::write_actions{
              actions::set_ipv4_src{"127.0.0.1"_ipv4}
            , actions::output{1}
        };

        auto const sut = v13::instruction_set{write_actions};

        BOOST_TEST(sut.length() == write_actions.length());
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 1);
        BOOST_TEST((sut.begin() != sut.end()));
        BOOST_TEST((*sut.begin() == write_actions));

        BOOST_TEST(
                (sut.find(v13::get_order(write_actions)) != sut.end()));
        BOOST_TEST((sut.at(v13::get_order(write_actions)) == write_actions));
        BOOST_TEST((sut.at<instructions::write_actions>() == write_actions));
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::write_actions>()));
        BOOST_TEST((sut.get<instructions::write_actions>() == write_actions));

        auto const goto_table_order
            = v13::instruction_order<instructions::goto_table>::value;
        BOOST_TEST(!(sut.find<instructions::apply_actions>()));
        BOOST_TEST((sut.find(goto_table_order) == sut.end()));
        BOOST_CHECK_THROW(
                  sut.at<instructions::apply_actions>(), std::out_of_range);
        BOOST_CHECK_THROW(sut.at(goto_table_order), std::out_of_range);
    }

    BOOST_AUTO_TEST_CASE(construct_from_multiple_instructions_test)
    {
        auto const write_actions = instructions::write_actions{
              actions::set_ipv4_src{"127.0.0.1"_ipv4}
            , actions::set_ipv4_dst{"127.0.0.2"_ipv4}
        }; // 8 + 16 + 16 = 40
        auto const clear_actions = instructions::clear_actions{}; // 8
        auto const apply_actions = instructions::apply_actions{
              actions::set_eth_type{0x0800}
            , actions::set_eth_src{"\x01\x02\x03\x04\x05\x06"_mac}
        }; // 8 + 16 + 16 = 40
        auto const goto_table = instructions::goto_table{4}; // 8

        auto const sut = v13::instruction_set{
            write_actions, clear_actions, apply_actions, goto_table
        };

        BOOST_TEST(sut.length() == 40 + 8 + 40 + 8);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 4);

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::write_actions>()));
        BOOST_TEST((sut.get<instructions::write_actions>() == write_actions));

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::clear_actions>()));
        BOOST_TEST((sut.get<instructions::clear_actions>() == clear_actions));

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::apply_actions>()));
        BOOST_TEST((sut.get<instructions::apply_actions>() == apply_actions));

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::goto_table>()));
        BOOST_TEST((sut.get<instructions::goto_table>() == goto_table));
    }

    BOOST_AUTO_TEST_CASE(construct_from_same_instructions_test)
    {
        auto const write_actions1 = instructions::write_actions{
              actions::set_ipv4_src{"127.0.0.1"_ipv4}
            , actions::set_ipv4_dst{"127.0.0.2"_ipv4}
        }; // 8 + 16 + 16 = 40
        auto const clear_actions = instructions::clear_actions{}; // 8
        auto const write_actions2 = instructions::write_actions{
              actions::set_queue{4}
            , actions::set_ipv4_src{"127.0.0.3"_ipv4}
        }; // 8 + 8 + 16 = 32
        auto const goto_table = instructions::goto_table{4}; // 8

        auto const sut = v13::instruction_set{
            write_actions1, clear_actions, write_actions2, goto_table
        };

        BOOST_TEST(sut.length() == 40 + 8 + 8);
        BOOST_TEST(!sut.empty());
        BOOST_TEST(sut.size() == 3);

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::write_actions>()));
        BOOST_TEST((sut.get<instructions::write_actions>() == write_actions1));

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::clear_actions>()));
        BOOST_TEST((sut.get<instructions::clear_actions>() == clear_actions));

        BOOST_TEST_REQUIRE(bool(sut.find<instructions::goto_table>()));
        BOOST_TEST((sut.get<instructions::goto_table>() == goto_table));
    }

    BOOST_FIXTURE_TEST_CASE(copy_construct_test, instruction_set_fixture)
    {
        auto const copy = sut;

        BOOST_TEST((copy == sut));
    }

    BOOST_FIXTURE_TEST_CASE(move_construct_test, instruction_set_fixture)
    {
        auto src = sut;

        auto const copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == 0);
        BOOST_TEST(src.empty());
    }

    BOOST_FIXTURE_TEST_CASE(copy_assignment_test, instruction_set_fixture)
    {
        auto copy = v13::instruction_set{};

        copy = sut;

        BOOST_TEST((copy == sut));
        BOOST_CHECK_EQUAL(copy.length(), sut.length());
    }

    BOOST_FIXTURE_TEST_CASE(move_assignment_test, instruction_set_fixture)
    {
        auto src = sut;
        auto copy = v13::instruction_set{};

        copy = std::move(src);

        BOOST_TEST((copy == sut));
        BOOST_TEST(src.length() == 0);
        BOOST_TEST(src.empty());
    }

    BOOST_AUTO_TEST_CASE(equality_test)
    {
        auto const sut = v13::instruction_set{
              instructions::goto_table{1}
            , instructions::apply_actions{actions::output{1}, actions::group{2}}
        };
        auto const diff_value1 = v13::instruction_set{
              instructions::goto_table{2}
            , instructions::apply_actions{actions::output{1}, actions::group{2}}
        };
        auto const diff_value2 = v13::instruction_set{
              instructions::goto_table{1}
            , instructions::apply_actions{actions::output{2}, actions::group{2}}
        };
        auto const diff_type1 = v13::instruction_set{
              instructions::goto_table{1}
            , instructions::write_actions{actions::output{1}, actions::group{2}}
        };
        auto const diff_type2 = v13::instruction_set{
              instructions::write_metadata{1}
            , instructions::apply_actions{actions::output{1}, actions::group{2}}
        };
        auto const empty = v13::instruction_set{};
        auto same_value_after_insert1 = v13::instruction_set{
              instructions::goto_table{1}
        };
        same_value_after_insert1.insert(instructions::apply_actions{
                actions::output{1}, actions::group{2}});
        auto same_value_after_insert2 = v13::instruction_set{
              instructions::apply_actions{actions::output{1}, actions::group{2}}
        };
        same_value_after_insert2.insert(instructions::goto_table{1});
        auto same_value_after_assign1 = v13::instruction_set{
              instructions::goto_table{2}
            , instructions::apply_actions{actions::output{1}, actions::group{2}}
        };
        same_value_after_assign1.assign(instructions::goto_table{1});
        auto same_value_after_assign2 = v13::instruction_set{
              instructions::goto_table{1}
            , instructions::apply_actions{actions::output{2}, actions::group{1}}
        };
        same_value_after_assign2.assign(instructions::apply_actions{
                actions::output{1}, actions::group{2}});

        BOOST_TEST((sut == sut));
        BOOST_TEST((sut != diff_value1));
        BOOST_TEST((sut != diff_value2));
        BOOST_TEST((sut != diff_type1));
        BOOST_TEST((sut != diff_type2));
        BOOST_TEST((sut != empty));
        BOOST_TEST((sut == same_value_after_insert1));
        BOOST_TEST((sut == same_value_after_insert2));
        BOOST_TEST((sut == same_value_after_assign1));
        BOOST_TEST((sut == same_value_after_assign2));
    }


    BOOST_FIXTURE_TEST_SUITE(modification_test, instruction_set_fixture)

    BOOST_AUTO_TEST_CASE(insert_new_field_test)
    {
        auto const write_metadata = instructions::write_metadata{1, 2};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(write_metadata);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == write_metadata));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + write_metadata.length());
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::write_metadata>()));
        BOOST_TEST((sut.get<instructions::write_metadata>() == write_metadata));
    }

    BOOST_AUTO_TEST_CASE(insert_existing_field_test)
    {
        auto const apply_actions = instructions::apply_actions{
            actions::set_ipv4_src{"127.0.0.1"_ipv4}, actions::pop_vlan{}
        };
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert(apply_actions);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first != apply_actions));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::apply_actions>()));
        BOOST_TEST((sut.get<instructions::apply_actions>() != apply_actions));
    }

    BOOST_AUTO_TEST_CASE(assign_new_field_test)
    {
        auto const write_metadata = instructions::write_metadata{1, 2};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.assign(write_metadata);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first != write_metadata));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST(!(sut.find<instructions::write_metadata>()));
    }

    BOOST_AUTO_TEST_CASE(assign_existing_field_test)
    {
        auto const apply_actions = instructions::apply_actions{
            actions::set_ipv4_src{"127.0.0.1"_ipv4}, actions::pop_vlan{}
        };
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const assigned_instruction_length
            = sut.at<instructions::apply_actions>().length();

        auto const result = sut.assign(apply_actions);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == apply_actions));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length()
                == (before_length
                  - assigned_instruction_length
                  + apply_actions.length()));
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::apply_actions>()));
        BOOST_TEST((sut.get<instructions::apply_actions>() == apply_actions));
    }

    BOOST_AUTO_TEST_CASE(insert_or_assign_new_field_test)
    {
        auto const write_metadata = instructions::write_metadata{1, 2};
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.insert_or_assign(write_metadata);

        BOOST_TEST(result.second);
        BOOST_TEST((*result.first == write_metadata));
        BOOST_TEST(sut.size() == before_size + 1);
        BOOST_TEST(sut.length() == before_length + write_metadata.length());
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::write_metadata>()));
        BOOST_TEST((sut.get<instructions::write_metadata>() == write_metadata));
    }

    BOOST_AUTO_TEST_CASE(insert_or_assign_existing_field_test)
    {
        auto const apply_actions = instructions::apply_actions{
            actions::set_ipv4_src{"127.0.0.1"_ipv4}, actions::pop_vlan{}
        };
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const assigned_instruction_length
            = sut.at<instructions::apply_actions>().length();

        auto const result = sut.insert_or_assign(apply_actions);

        BOOST_TEST(!result.second);
        BOOST_TEST((*result.first == apply_actions));
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length()
                == (before_length
                  - assigned_instruction_length
                  + apply_actions.length()));
        BOOST_TEST_REQUIRE(bool(sut.find<instructions::apply_actions>()));
        BOOST_TEST((sut.get<instructions::apply_actions>() == apply_actions));
    }

    BOOST_AUTO_TEST_CASE(swap_test)
    {
        auto tmp = v13::instruction_set{instructions::clear_actions{}};
        auto const tmp_length = tmp.length();
        auto const tmp_size = tmp.size();
        auto const sut_length = sut.length();
        auto const sut_size = sut.size();

        sut.swap(tmp);

        BOOST_TEST(sut.length() == tmp_length);
        BOOST_TEST(sut.size() == tmp_size);
        BOOST_TEST(tmp.length() == sut_length);
        BOOST_TEST(tmp.size() == sut_size);
    }

    BOOST_AUTO_TEST_CASE(erase_new_field_test)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();

        auto const result = sut.erase<instructions::write_metadata>();

        BOOST_TEST(result == 0);
        BOOST_TEST(sut.size() == before_size);
        BOOST_TEST(sut.length() == before_length);
        BOOST_TEST(!sut.find<instructions::write_metadata>());
    }

    BOOST_AUTO_TEST_CASE(erase_existing_field_test)
    {
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_instruction_length
            = sut.at<instructions::apply_actions>().length();

        auto const result = sut.erase<instructions::apply_actions>();

        BOOST_TEST(result == 1);
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_instruction_length);
        BOOST_TEST(!sut.find<instructions::apply_actions>());
    }

    BOOST_AUTO_TEST_CASE(erase_by_iterator_test)
    {
        auto it = std::prev(sut.end());
        auto const erased_instruction_order = v13::get_order(*it);
        auto const before_size = sut.size();
        auto const before_length = sut.length();
        auto const erased_instruction_length = it->length();

        auto const result = sut.erase(it);

        BOOST_TEST((result == sut.end()));
        BOOST_TEST(sut.size() == before_size - 1);
        BOOST_TEST(sut.length() == before_length - erased_instruction_length);
        BOOST_TEST((sut.find(erased_instruction_order) == sut.end()));
    }

    BOOST_AUTO_TEST_CASE(clear_test)
    {
        sut.clear();

        BOOST_TEST(sut.length() == 0);
        BOOST_TEST(sut.empty());
        BOOST_TEST(sut.size() == 0);
        BOOST_TEST((sut.begin() == sut.end()));
    }

    BOOST_AUTO_TEST_SUITE_END() // modification_test

    BOOST_FIXTURE_TEST_CASE(encode_test, instruction_set_fixture)
    {
        auto buffer = std::vector<unsigned char>{};

        sut.encode(buffer);

        BOOST_TEST(buffer.size() == sut.length());
        BOOST_TEST(buffer == binary, boost::test_tools::per_element{});
    }

    BOOST_FIXTURE_TEST_CASE(decode_test, instruction_set_fixture)
    {
        auto it = binary.begin();
        auto const it_end = binary.end();

        auto const instruction_set = v13::instruction_set::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((instruction_set == sut));
    }

    BOOST_FIXTURE_TEST_CASE(
            decode_different_order_instruction_test, instruction_set_fixture)
    {
        auto it = diff_order_binary.begin();
        auto const it_end = diff_order_binary.end();

        auto const instruction_set = v13::instruction_set::decode(it, it_end);

        BOOST_TEST((it == it_end));
        BOOST_TEST((instruction_set == sut));
    }

    BOOST_FIXTURE_TEST_CASE(
            decode_duplicated_instruction_test, instruction_set_fixture)
    {
        auto it = duplicated_binary.begin();
        auto const it_end = duplicated_binary.end();

        BOOST_CHECK_THROW(
                  v13::instruction_set::decode(it, it_end)
                , std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END() // instruction_set_test
BOOST_AUTO_TEST_SUITE_END() // common_type_test
