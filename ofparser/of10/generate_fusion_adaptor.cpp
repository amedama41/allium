#include <fstream>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include <boost/range/irange.hpp>
#include "generate_fusion_adaptor.hpp"

namespace {

    auto maybe_add_qualified_id(std::string const& version, std::string const& type)
        -> std::string
    {
        if (type.find("uint") == 0) {
            return "std::" + type;
        }
        if (type.find("ofp_") == 0) {
            return (boost::format{"canard::network::openflow::v%1%::detail::%2%"} % version % type).str();
        }
        return type;
    }

    auto fusion_adaptor_code(std::string const& version, std::vector<struct_info> const& sinfos)
        -> std::string
    {
        std::ostringstream oss;
        for (auto&& sinfo : sinfos) {
            oss << "BOOST_FUSION_ADAPT_STRUCT(" << std::endl;
            oss << boost::format{"    canard::network::openflow::v%1%::detail::%2%,"}
                % version % sinfo.name
                << std::endl;
            for (auto& dinfo : sinfo.member_info) {
                if (dinfo.type == "char") {
                    continue;
                }
                if (dinfo.declarator.is_array) {
                    for (auto i : boost::irange(0UL, dinfo.declarator.array_length)) {
                        oss << boost::format{"    (%1%, %2%[%3%])"}
                            % maybe_add_qualified_id(version, dinfo.type) % dinfo.declarator.name % i
                            << std::endl;
                    }
                }
                else {
                    oss << boost::format("    (%1%, %2%)")
                        % maybe_add_qualified_id(version, dinfo.type) % dinfo.declarator.name
                        << std::endl;
                }
            }
            oss << ")\n" << std::endl;
        }
        return oss.str();
    }

}

void generate_fusion_adaptor(std::string const& version, std::vector<struct_info> const& sinfo)
{
    std::ofstream ofs{
        (boost::format{"../../include/canard/network/protocol/openflow/v%1%/detail/fusion_adaptor.hpp"}
         % version).str()
    };
    ofs << boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V%1%_FUSION_ADATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V%1%_FUSION_ADATOR_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <canard/network/protocol/openflow/v%1%/openflow.hpp>

%2%
#endif // CANARD_NETWORK_OPENFLOW_V%1%_FUSION_ADATOR_HPP
)"
    } % version % fusion_adaptor_code(version, sinfo);
}

