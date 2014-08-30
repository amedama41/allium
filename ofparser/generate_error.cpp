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

    auto error_category_code(enum_info const& einfo)
        -> std::string
    {
        auto error_category_format = boost::format{
R"(
        class %1%_category
            : public boost::system::error_category
        {
        public:
            static ofp_error_type const error_type = %2%;

            auto name() const noexcept
                -> char const*
            {
                return "%1%";
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
                return v13::to_string(ofp_%1%_code(ev - 1));
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
                % to_lower_case(std::regex_replace(member.name, std::regex{R"(^OFPET_(.*))"}, R"($1)"))
                % member.name;
        }
        return oss.str();
    }

    auto generate_code(enum_info const& einfo, boost::format code_format)
        -> std::string
    {
        std::ostringstream oss{};
        for (auto const& member : einfo.member_info) {
            if (member.name.find("EXPERIMENTER") != std::string::npos) {
                continue;
            }
            oss << code_format
                % to_lower_case(std::regex_replace(member.name, std::regex{R"(^OFPET_(.*))"}, R"($1)"));
        }
        return oss.str();
    }

    auto get_category_code(enum_info const& einfo)
        -> std::string
    {
        auto get_category_format = boost::format{
R"(
    inline auto %1%_category()
        -> boost::system::error_category&
    {
        static detail::%1%_category instance{};
        return instance;
    }
)"
        };
        return generate_code(einfo, get_category_format);
    }

    auto make_error_code_code(enum_info const& einfo)
        -> std::string
    {
        auto make_error_code_format = boost::format{
R"(
    inline auto make_error_code(ofp_%1%_code const e)
        -> boost::system::error_code
    {
        return {e + 1, v13::%1%_category()};
    }
)"
        };
        return generate_code(einfo, make_error_code_format);
    }

    auto is_error_code_enum_code(enum_info const& einfo)
        -> std::string
    {
        auto is_error_code_enum_format = boost::format{
R"(
    template <> struct is_error_code_enum<canard::network::openflow::v13::ofp_%1%_code>
    {
        static bool const value = true;
    };
)"
        };
        return generate_code(einfo, is_error_code_enum_format);
    }
}

void generate_error(std::vector<enum_info> const& einfos)
{
    auto const it = boost::find_if(einfos, [](auto const& einfo) { return einfo.name == "ofp_error_type"; });
    if (it == einfos.end()) {
        std::cout << __func__ << ": no ofp_error_type" << std::endl;
        return;
    }

    auto const ec_code = error_category_code(*it);
    auto const gc_code = get_category_code(*it);
    auto const mec_code = make_error_code_code(*it);
    auto const iece_code = is_error_code_enum_code(*it);

    std::ofstream ofs{"../include/canard/network/protocol/openflow/v13/error.hpp"};

    auto error_file_format = boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V13_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ERROR_HPP

#include <string>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/io/enum_to_string.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {
%1%
    } // namespace detail
%2%
%3%
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

namespace boost {
namespace system {
%4%
} // namespace system
} // namespace boost

#endif // CANARD_NETWORK_OPENFLOW_V13_ERROR_HPP
)"
    };
    ofs << error_file_format
        % ec_code % gc_code % mec_code % iece_code;
}

