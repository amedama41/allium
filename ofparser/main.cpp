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

auto const macro_to_type = std::unordered_map<std::string, std::string>{
        {"OFP_VERSION", "std::uint8_t"}
      , {"OFP_TCP_PORT", "std::uint16_t"}
      , {"OFP_SSL_PORT", "std::uint16_t"}
      , {"OFP_DEFAULT_MISS_SEND_LEN", "std::uint16_t"}
      , {"OFP_FLOW_PERMANENT", "std::uint16_t"}
      , {"OFP_DEFAULT_PRIORITY", "std::uint16_t"}
      , {"OFP_NO_BUFFER", "std::uint32_t"}
      , {"OFPQ_ALL", "std::uint32_t"}
      , {"OFPQ_MIN_RATE_UNCFG", "std::uint16_t"}
      , {"OFPQ_MAX_RATE_UNCFG", "std::uint16_t"}
};

template <class OStream>
static void generate_enum(OStream& os, std::vector<enum_info> const& einfos)
{
    auto enum_code = boost::format{
R"(    enum %1%
    {
%2%
    };)"
    };
    auto enumerator_code = boost::format{
R"(        %1%%|32t| = %2%,)"
    };

    for (auto&& e : einfos) {
        std::ostringstream oss{};
        for (auto&& emem : e.member_info) {
            if (emem.value.empty()) {
                oss << "        " << emem.name << ",\n";
            }
            else {
                oss << enumerator_code % emem.name % emem.value << '\n';
            }
        }
        auto enumerator_list = oss.str();
        enumerator_list.pop_back();
        os << enum_code % e.name % enumerator_list << "\n\n";
    }
}

static auto maybe_add_std(std::string const& type)
    -> std::string
{
    if (type.find("uint") == 0) {
        return "std::" + type;
    }
    return type;
}

template <class OStream, class Map>
static void generate_struct(OStream& os, std::vector<struct_info> const& sinfos, Map const& struct_to_size)
{
    auto struct_code = boost::format{
R"(        struct %1%
        {
%2%
        };)"
    };

    for (auto&& s : sinfos) {
        std::ostringstream oss{};
        auto has_ofp_match = false;
        for (auto&& smem : s.member_info) {
            if (smem.type == "ofp_match") {
                has_ofp_match = true;
                continue;
            }
            if (smem.declarator.is_array) {
                if (smem.declarator.array_length == 0) {
                    continue;
                }
                oss << boost::format{R"(            %1%%|24t| %2%[%3%];)"}
                        % maybe_add_std(smem.type) % smem.declarator.name % smem.declarator.array_length << '\n';
            }
            else {
                oss << boost::format{R"(            %1%%|24t| %2%;)"}
                        % maybe_add_std(smem.type) % smem.declarator.name << '\n';
            }
        }
        auto declarator_list = oss.str();
        declarator_list.pop_back();
        os << struct_code % s.name % declarator_list << "\n";
        auto const it = struct_to_size.find(s.name);
        if (it != struct_to_size.end()) {
            os << boost::format{R"(        static_assert(sizeof(%1%) == %2%, "");)"}
                    % it->first
                    % (it->second - (has_ofp_match ? struct_to_size.find("ofp_match")->second : 0))
               << '\n';
        }
        os << '\n';
    }
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
    extern void generate_byteorder(std::vector<struct_info> const& sinfos);
    generate_byteorder(infos.first);
    generate_enum_to_string(infos.second);
    generate_error(infos.second);
    generate_fusion_adaptor(infos.first);
    std::ofstream ofs{"../include/canard/network/protocol/openflow/v13/openflow.hpp"};
    ofs <<
R"(#ifndef CANARD_NETWORK_OPENFLOW_V13_OPENFLOW_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OPENFLOW_HPP

#include <cstdint>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

)";
    generate_struct(ofs, infos.first, struct_to_size);
    ofs <<
R"(    } // namespace detail
)" << '\n';
    generate_enum(ofs, infos.second);
    for (auto&& macro : constant_macros) {
        ofs << boost::format{R"(    constexpr auto const %1% = %2%;)"} % macro.first % macro.second << '\n';
    }
    ofs <<
R"(
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OPENFLOW_HPP
)";

    return 0;
}
