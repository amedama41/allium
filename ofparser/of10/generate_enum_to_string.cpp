#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/range/algorithm/find.hpp>
#include "generate_enum_to_string.hpp"

namespace {
    auto const ignored_enumerators = std::vector<std::string>{
          "OFPPS_STP_MASK"
        , "OFPFW_NW_SRC_SHIFT", "OFPFW_NW_SRC_BITS"
        , "OFPFW_NW_DST_SHIFT", "OFPFW_NW_DST_BITS"
    };

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
            if (boost::find(ignored_enumerators, enumerator.name) != ignored_enumerators.end()) {
                continue;
            }
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

void generate_enum_to_string(std::string const& version, std::vector<enum_info> const& einfos)
{
    std::ostringstream oss{};
    for (auto const& enumeration : einfos) {
        oss << to_string_function_code(enumeration
                , (enumeration.name == (version == "10" ? "ofp_port" : "ofp_port_no")
                    ? "switch_port"
                    : enumeration.name == "ofp_table"
                    ? "table_id"
                    : enumeration.name == "ofp_group"
                    ? "group_id"
                    : "unkonw_" + enumeration.name
                  ));
    }

    std::ofstream ofs{
        (boost::format{"../../include/canard/network/protocol/openflow/v%1%/io/enum_to_string.hpp"}
         % version).str()
    };
    ofs << boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V%1%_ENUM_TO_STRING_HPP
#define CANARD_NETWORK_OPENFLOW_V%1%_ENUM_TO_STRING_HPP

#include <string>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v%1%/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v%1% {
%2%
} // namespace v%1%
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V%1%_ENUM_TO_STRING_HPP
)"
    } % version % oss.str();

}

