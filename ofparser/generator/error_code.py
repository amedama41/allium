from future_builtins import map, filter

def _strip_OFPET(name):
    return name.replace('OFPET_', '').lower()

def _generate_error_category_classes(version, error_type_enum, ignore_types):
    return '\n\n'.join(map(
        lambda error: (
"""\
        class {name}_category
            : public boost::system::error_category
        {{
        public:
            static net::ofp::v{version}::protocol::ofp_error_type const error_type
                = net::ofp::v{version}::protocol::{value};

            auto name() const noexcept
                -> char const*
            {{
                return "{name}";
            }}

            auto default_error_condition(int const ev) const noexcept
                -> boost::system::error_condition
            {{
                return {{ev + 1, *this}};
            }}

            auto equivalent(
                    boost::system::error_code const& code
                  , int const condition) const noexcept
                -> bool
            {{
                return *this == code.category() && code.value() == condition + 1;
            }}

            auto message(int const ev) const
                -> std::string
            {{
                std::ostringstream oss;
                oss << net::ofp::v{version}::protocol::ofp_{name}_code(ev - 1);
                return oss.str();
            }}
        }};\
""".format(version=version, name=_strip_OFPET(error.displayname), value=error.displayname)),
        filter(lambda enum: enum.displayname not in ignore_types, error_type_enum.get_children())))

def _generate_error_category_funcs(version, error_type_enum, ignore_types):
    return '\n\n'.join(map(
        lambda error: (
"""\
    inline auto {name}_category()
        -> boost::system::error_category&
    {{
        static v{version}_detail::{name}_category instance{{}};
        return instance;
    }}\
""".format(version=version, name=_strip_OFPET(error.displayname))),
        filter(lambda enum: enum.displayname not in ignore_types, error_type_enum.get_children())))

def _generate_make_error_code_funcs(version, error_type_enum, ignore_types):
    return '\n\n'.join(map(
        lambda error: (
"""\
    inline auto make_error_code(protocol::ofp_{name}_code const e)
        -> boost::system::error_code
    {{
        return {{e + 1, network::openflow::v{version}::{name}_category()}};
    }}\
""".format(version=version, name=_strip_OFPET(error.displayname))),
        filter(lambda enum: enum.displayname not in ignore_types, error_type_enum.get_children())))

def _generate_is_error_code_enum_specialization(version, error_type_enum, ignore_types):
    return '\n\n'.join(map(
        lambda error: (
"""\
    template <>
    struct is_error_code_enum<
        canard::net::ofp::v{version}::protocol::ofp_{name}_code
    >
    {{
        static bool const value = true;
    }};\
""".format(version=version, name=_strip_OFPET(error.displayname))),
        filter(lambda enum: enum.displayname not in ignore_types, error_type_enum.get_children())))


def generate(collector, ignore_types):
    error_type = collector.enum_decls['ofp_error_type']

    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_ERROR_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_ERROR_HPP

#include <sstream>
#include <string>
#include <boost/system/error_code.hpp>
#include <canard/network/openflow/v{version}/io/openflow.hpp>

namespace canard {{

namespace network {{
namespace openflow {{
namespace v{version} {{

    namespace v{version}_detail {{

{error_category_classes}

    }} // namespace v{version}_detail

{error_category_funcs}

}} // namespace v{version}
}} // namespace openflow
}} // namespace network

namespace net {{
namespace ofp {{
namespace v{version} {{

{make_error_code_funcs}

}} // namespace v{version}
}} // namespace ofp
}} // namespace net

}} // namespace canard

namespace boost {{
namespace system {{

{is_error_code_enum_specialization}

}} // namespace system
}} // namespace boost

#endif // CANARD_NETWORK_OPENFLOW_V{version}_ERROR_HPP\
""".format(
    version=collector.version,
    error_category_classes=_generate_error_category_classes(collector.version, error_type, ignore_types),
    error_category_funcs=_generate_error_category_funcs(collector.version, error_type, ignore_types),
    make_error_code_funcs=_generate_make_error_code_funcs(collector.version, error_type, ignore_types),
    is_error_code_enum_specialization=_generate_is_error_code_enum_specialization(collector.version, error_type, ignore_types)))

