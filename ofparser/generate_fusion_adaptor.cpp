#include <fstream>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include <boost/range/irange.hpp>
#include "generate_fusion_adaptor.hpp"

namespace {

    auto maybe_add_qualified_id(std::string const& type)
        -> std::string
    {
        if (type.find("uint") == 0) {
            return "std::" + type;
        }
        if (type.find("ofp_") == 0) {
            return "canard::network::openflow::v13::detail::" + type;
        }
        return type;
    }

    auto fusion_adaptor_code(std::vector<struct_info> const& sinfos)
        -> std::string
    {
        std::ostringstream oss;
        for (auto&& sinfo : sinfos) {
            oss << "BOOST_FUSION_ADAPT_STRUCT(" << std::endl;
            oss << "    canard::network::openflow::v13::detail::" << sinfo.name << "," << std::endl;
            for (auto& dinfo : sinfo.member_info) {
                if (dinfo.type == "char" || dinfo.type == "ofp_match") {
                    continue;
                }
                if (dinfo.declarator.is_array) {
                    for (auto i : boost::irange(0UL, dinfo.declarator.array_length)) {
                        oss << boost::format{"    (%1%, %2%[%3%])"} % maybe_add_qualified_id(dinfo.type) % dinfo.declarator.name % i << std::endl;
                    }
                }
                else {
                    oss << boost::format("    (%1%, %2%)") % maybe_add_qualified_id(dinfo.type) % dinfo.declarator.name << std::endl;
                }
            }
            oss << ")\n" << std::endl;
        }
        return oss.str();
    }

}

void generate_fusion_adaptor(std::vector<struct_info> const& sinfo)
{
    auto fusion_adaptor_file_format = boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V13_FUSION_ADATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FUSION_ADATOR_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

%1%
#endif // CANARD_NETWORK_OPENFLOW_V13_FUSION_ADATOR_HPP
)"
    };

    std::ofstream ofs{"../include/canard/network/protocol/openflow/v13/detail/fusion_adaptor.hpp"};
    ofs << fusion_adaptor_file_format % fusion_adaptor_code(sinfo);
}

