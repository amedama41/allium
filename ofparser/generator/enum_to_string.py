from future_builtins import map, filter

def _case_for_each_member(members, ignore_to_string_enums):
    return '\n'.join(map(
        lambda mem: '        case protocol::{name}: return "{name}";'.format(name=mem.displayname),
        filter(lambda mem: mem.displayname not in ignore_to_string_enums, members)))

def _default_string(enum_name):
    if enum_name == 'ofp_port' or enum_name == 'ofp_port_no':
        return 'switch_port'
    if enum_name == 'ofp_table':
        return 'table_id'
    if enum_name == 'group_id':
        return 'group_id'
    return 'unkonw_' + enum_name

def _generate_to_string_funcs(enum_decls, ignore_to_string_enums):
    return '\n\n'.join(map(
        lambda (name, enum): (
"""\
    inline auto to_string(protocol::{name} const value)
        -> std::string
    {{
        switch (value) {{
{case}
        default: return boost::str(boost::format{{"{default}(%1%)"}} % value);
        }}
    }}\
""".format(
    name=name, case=_case_for_each_member(enum.get_children(), ignore_to_string_enums),
    default=_default_string(name))),
        enum_decls.items()))


def generate(collector, ignore_to_string_enums):
    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_ENUM_TO_STRING_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_ENUM_TO_STRING_HPP

#include <string>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v{version}/openflow.hpp>

namespace canard {{
namespace network {{
namespace openflow {{
namespace v{version} {{

{to_string_funcs}

}} // namespace v{version}
}} // namespace openflow
}} // namespace network
}} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V{version}_ENUM_TO_STRING_HPP\
""".format(version=collector.version, to_string_funcs=_generate_to_string_funcs(collector.enum_decls, ignore_to_string_enums)))

