#include <fstream>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include "generate_enum_to_string.hpp"

namespace {
    auto to_string_function_code(enum_info const& enumeration, std::string const& default_string)
        -> std::string
    {
        std::ostringstream oss{};
        oss << boost::format{
R"(
    inline auto to_string(%1% const value)
        -> std::string
    {
        switch (value) {
)"
        } % enumeration.name;

        auto case_format = boost::format{
R"_(        case %1%: %|48t|return "%1%";
)_"
        };
        for (auto const& enumerator : enumeration.member_info) {
            oss << case_format % enumerator.name;
        }

        oss << boost::format{
R"_(        default: %|48t|return (boost::format{"%1%(%%1%%)"} %% value).str();
        }
    }
)_"
        } % default_string;
        return oss.str();
    }
}

void generate_enum_to_string(std::vector<enum_info> const& einfos)
{
    auto enum_to_string_file_format = boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V13_ENUM_TO_STRING_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ENUM_TO_STRING_HPP

#include <string>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
%1%
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ENUM_TO_STRING_HPP
)"
    };

    std::ostringstream oss{};

    for (auto const& enumeration : einfos) {
        oss << to_string_function_code(enumeration
                , (enumeration.name == "ofp_port_no"
                    ? "switch_port"
                    : enumeration.name == "ofp_table"
                    ? "table_id"
                    : enumeration.name == "ofp_group"
                    ? "group_id"
                    : "unkonw_" + enumeration.name
                  ));
    }

    std::ofstream ofs{"../include/canard/network/protocol/openflow/v13/io/enum_to_string.hpp"};
    ofs << enum_to_string_file_format % oss.str();
}

