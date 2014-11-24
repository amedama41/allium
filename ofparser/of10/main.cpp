#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/range/irange.hpp>
#include <boost/wave/cpp_exceptions.hpp>
#include "code_information.hpp"
#include "comment_to_space.hpp"
#include "generate_enum_to_string.hpp"
#include "generate_error.hpp"
#include "generate_fusion_adaptor.hpp"
#include "parse.hpp"
#include "preprocess.hpp"
#include "generate_ofheader.cpp"
#include "generate_byteorder.cpp"
#include "generate_fusion_adaptor.cpp"
#include "generate_enum_to_string.cpp"
#include "generate_error.cpp"

static auto type(std::string const& name)
    -> std::string
{
    auto const macro_to_type = std::unordered_map<std::string, std::string>{
            {"OFP_VERSION", "std::uint8_t"}
          , {"OFP_MAX_TABLE_NAME_LEN", "std::size_t"}
          , {"OFP_MAX_PORT_NAME_LEN" , "std::size_t"}
          , {"OFP_TCP_PORT", "std::uint16_t"}
          , {"OFP_SSL_PORT", "std::uint16_t"}
          , {"OFP_ETH_ALEN", "std::size_t"}
          , {"OFP_DEFAULT_MISS_SEND_LEN", "std::uint16_t"}
          , {"OFP_VLAN_NONE", "std::uint16_t"}
          , {"OFP_FLOW_PERMANENT", "std::uint16_t"}
          , {"OFP_DEFAULT_PRIORITY", "std::uint16_t"}
          , {"DESC_STR_LEN", "std::size_t"}
          , {"SERIAL_NUM_LEN", "std::size_t"}
          , {"OFPQ_ALL", "std::uint32_t"}
          , {"OFPQ_MIN_RATE_UNCFG", "std::uint16_t"}
    };
    auto const it = macro_to_type.find(name);
    if (it == macro_to_type.end()) {
        return "auto";
    }
    return it->second;
}


int main(int argc, char const* argv[])
{
    if (argc != 2) {
        std::cout << boost::format("Usage: %1% <openflow header file>") % argv[0] << std::endl;
        return 0;
    }
    std::ifstream instream{argv[1]};
    auto input = std::string{
          std::istreambuf_iterator<char>{instream.rdbuf()}
        , std::istreambuf_iterator<char>{}
    };
    auto struct_to_size = std::unordered_map<std::string, std::size_t>{};
    auto constant_macros = std::vector<std::pair<std::string, std::string>>{};

    try {
        std::tie(input, struct_to_size, constant_macros) = ofparser::preprocess(input, argv[1]);
    }
    catch (boost::wave::cpp_exception& e) {
        std::cerr << boost::format("%1%(%2%):%3%") % e.file_name() % e.line_no() % e.description() << std::endl;
    }

    input = ofparser::comment_to_space(input);

    auto infos = ofparser::parse(input);
    generate_byteorder("10", infos.first);
    generate_enum_to_string("10", infos.second);
    generate_error("10", infos.second);
    generate_fusion_adaptor("10", infos.first);
    std::ofstream ofs{"../../include/canard/network/protocol/openflow/v10/openflow.hpp"};
    ofs <<
R"(#ifndef CANARD_NETWORK_OPENFLOW_V10_OPENFLOW_HPP
#define CANARD_NETWORK_OPENFLOW_V10_OPENFLOW_HPP

#include <cstddef>
#include <cstdint>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace detail {
)";
    generate_struct(ofs, infos.first, struct_to_size);
    ofs <<
R"(
    } // namespace detail

)";
    generate_enum(ofs, infos.second);
    ofs << '\n';
    for (auto&& macro : constant_macros) {
        ofs << boost::format{"    constexpr %1% const %2% = %3%;\n"}
            % type(macro.first) % macro.first % macro.second;
    }
    ofs <<
R"(
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_OPENFLOW_HPP
)";

    return 0;
}
