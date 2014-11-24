#include <sstream>
#include <vector>
#include "../code_information.hpp"

static auto maybe_add_std(std::string const& type)
    -> std::string
{
    if (type.find("uint") == 0) {
        return "std::" + type;
    }
    return type;
}

template <class Map>
static auto struct_size(std::string const& name, Map const& struct_to_size)
    -> std::size_t
{
    auto const it = struct_to_size.find(name);
    if (it == struct_to_size.end()) {
        return 8;
    }
    return it->second;
}

template <class OStream, class Map>
void generate_struct(OStream& os, std::vector<struct_info> const& sinfo, Map const& struct_to_size)
{
    for (auto&& s : sinfo) {
        std::ostringstream oss{};
        for (auto&& smem : s.member_info) {
            if (smem.declarator.is_array) {
                if (smem.declarator.array_length == 0) {
                    continue;
                }
                oss << boost::format{"%|12t|%1%  %|28t|%2%[%3%];\n"}
                        % maybe_add_std(smem.type) % smem.declarator.name % smem.declarator.array_length;
            }
            else {
                oss << boost::format{"%|12t|%1%  %|28t|%2%;\n"}
                        % maybe_add_std(smem.type) % smem.declarator.name;
            }
        }
        auto declarator_list = oss.str();
        declarator_list.pop_back(); // remove \n
        os << boost::format{
R"(
        struct %1%
        {
%2%
        };
        static_assert(sizeof(%1%) == %3%, "");
)"
        } % s.name % declarator_list % struct_size(s.name, struct_to_size);
    }
}

template <class OStream>
void generate_enum(OStream& os, std::vector<enum_info> const& einfo)
{
    for (auto&& e : einfo) {
        std::ostringstream oss{};
        for (auto&& emem : e.member_info) {
            if (emem.value.empty()) {
                oss << boost::format{"%|8t|%1%,\n"} % emem.name;
            }
            else {
                oss << boost::format{"%|8t|%1%%|32t| = %2%,\n"} % emem.name % emem.value;
            }
        }
        auto enumerator_list = oss.str();
        enumerator_list.pop_back();
        os << boost::format{
R"(
    enum %1%
    {
%2%
    };
)"
        }% e.name % enumerator_list;
    }
}

