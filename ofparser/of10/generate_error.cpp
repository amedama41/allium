#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include "generate_error.hpp"

namespace {
    auto to_lower_case(std::string str)
        -> std::string
    {
        boost::algorithm::to_lower(str);
        return str;
    }

    auto error_category_code(std::string const& version, enum_info const& einfo)
        -> std::string
    {
        auto error_category_format = boost::format{
R"(
        class %2%_category
            : public boost::system::error_category
        {
        public:
            static ofp_error_type const error_type = %3%;

            auto name() const noexcept
                -> char const*
            {
                return "%2%";
            }

            auto default_error_condition(int const ev) const noexcept
                -> boost::system::error_condition
            {
                return {ev + 1, *this};
            }

            auto equivalent(boost::system::error_code const& code, int const condition) const noexcept
                -> bool
            {
                return *this == code.category() && code.value() == condition + 1;
            }

            auto message(int const ev) const
                -> std::string
            {
                return v%1%::to_string(ofp_%2%_code(ev - 1));
            }
        };
)"
        };
        std::ostringstream oss{};
        for (auto const& member : einfo.member_info) {
            if (member.name.find("EXPERIMENTER") != std::string::npos) {
                continue;
            }
            oss << error_category_format
                % version
                % to_lower_case(std::regex_replace(member.name, std::regex{R"(^OFPET_(.*))"}, R"($1)"))
                % member.name;
        }
        return oss.str();
    }

    auto generate_code(enum_info const& einfo, std::string const& version, boost::format code_format)
        -> std::string
    {
        std::ostringstream oss{};
        for (auto const& member : einfo.member_info) {
            if (member.name.find("EXPERIMENTER") != std::string::npos) {
                continue;
            }
            oss << code_format
                % version
                % to_lower_case(std::regex_replace(member.name, std::regex{R"(^OFPET_(.*))"}, R"($1)"));
        }
        return oss.str();
    }

    auto get_category_code(std::string const& version, enum_info const& einfo)
        -> std::string
    {
        auto get_category_format = boost::format{
R"(
    inline auto %2%_category()
        -> boost::system::error_category&
    {
        static detail::%2%_category instance{};
        return instance;
    }
)"
        };
        return generate_code(einfo, version, get_category_format);
    }

    auto make_error_code_code(std::string const& version, enum_info const& einfo)
        -> std::string
    {
        auto make_error_code_format = boost::format{
R"(
    inline auto make_error_code(ofp_%2%_code const e)
        -> boost::system::error_code
    {
        return {e + 1, v%1%::%2%_category()};
    }
)"
        };
        return generate_code(einfo, version, make_error_code_format);
    }

    auto is_error_code_enum_code(std::string const& version, enum_info const& einfo)
        -> std::string
    {
        auto is_error_code_enum_format = boost::format{
R"(
    template <> struct is_error_code_enum<canard::network::openflow::v%1%::ofp_%2%_code>
    {
        static bool const value = true;
    };
)"
        };
        return generate_code(einfo, version, is_error_code_enum_format);
    }
}

void generate_error(std::string const& version, std::vector<enum_info> const& einfos)
{
    auto const it = boost::find_if(einfos, [](auto const& einfo) { return einfo.name == "ofp_error_type"; });
    if (it == einfos.end()) {
        std::cout << __func__ << ": no ofp_error_type" << std::endl;
        return;
    }

    auto const ec_code = error_category_code(version, *it);
    auto const gc_code = get_category_code(version, *it);
    auto const mec_code = make_error_code_code(version, *it);
    auto const iece_code = is_error_code_enum_code(version, *it);

    std::ofstream ofs{
        (boost::format{"../../include/canard/network/protocol/openflow/v%1%/error.hpp"}
         % version).str()
    };

    ofs << boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V%1%_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_V%1%_ERROR_HPP

#include <string>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v%1%/io/enum_to_string.hpp>
#include <canard/network/protocol/openflow/v%1%/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v%1% {

    namespace detail {
%2%
    } // namespace detail
%3%
%4%
} // namespace v%1%
} // namespace openflow
} // namespace network
} // namespace canard

namespace boost {
namespace system {
%5%
} // namespace system
} // namespace boost

#endif // CANARD_NETWORK_OPENFLOW_V%1%_ERROR_HPP
)"
    } % version % ec_code % gc_code % mec_code % iece_code;
}

