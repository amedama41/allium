#ifndef OFPARSER_CODE_INFORMATION
#define OFPARSER_CODE_INFORMATION

#include <cstddef>
#include <string>
#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

struct declarator_info
{
    std::string name;
    bool is_array;
    std::size_t array_length;
};
BOOST_FUSION_ADAPT_STRUCT(
        struct declarator_info,
        (std::string, name)
        (bool, is_array)
        (std::size_t, array_length)
)

struct declaration_info
{
    std::string type;
    declarator_info declarator;
};
BOOST_FUSION_ADAPT_STRUCT(
        struct declaration_info,
        (std::string, type)
        (declarator_info, declarator)
)

struct struct_info
{
    std::string name;
    std::vector<declaration_info> member_info;
};
BOOST_FUSION_ADAPT_STRUCT(
        struct struct_info,
        (std::string, name)
        (std::vector<declaration_info>, member_info)
)

struct enum_member_info
{
    std::string name;
    std::string value;
};
BOOST_FUSION_ADAPT_STRUCT(
        struct enum_member_info,
        (std::string, name)
        (std::string, value)
)

struct enum_info
{
    std::string name;
    std::vector<enum_member_info> member_info;
};
BOOST_FUSION_ADAPT_STRUCT(
        struct enum_info,
        (std::string, name)
        (std::vector<enum_member_info>, member_info)
)

#endif // OFPARSER_CODE_INFORMATION
