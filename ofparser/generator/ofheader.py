from future_builtins import map, filter
import itertools

def _maybe_qualify_std(type_name):
    return 'std::' + type_name if type_name.startswith('uint') else type_name

def _struct_size(struct_decl, is_skip_ofp_match):
    if is_skip_ofp_match:
        ofp_match = list(filter(lambda mem: mem.type == 'ofp_match', struct_decl.members))
        return struct_decl.size - (ofp_match[0].size if ofp_match else 0)
    return struct_decl.size

def _generate_struct_members(struct_members, is_skip_ofp_match):
    def to_member_decl(mem):
        if mem.is_array():
            return '            {type} {name}[{size}];'.format(
                    type=_maybe_qualify_std(mem.element_type), name=mem.name, size=mem.element_count)
        else:
            return '            {type} {name};'.format(
                    type=_maybe_qualify_std(mem.type), name=mem.name)
    return '\n'.join(map(
        to_member_decl,
        filter(
            lambda mem: not (
                (is_skip_ofp_match and mem.type == 'ofp_match')
                or mem.is_zero_elements_array()),
            struct_members)))

def _generate_struct_decls(struct_decls, is_skip_ofp_match=False):
    return '\n\n'.join(map(
        lambda (name, struct): (
"""\
        struct {name}
        {{
{members}
        }};
        static_assert(sizeof({name}) == {size}, "");\
""".format(
    name=name,
    members=_generate_struct_members(struct.members, is_skip_ofp_match),
    size=_struct_size(struct, is_skip_ofp_match))),
        struct_decls.items()))

def _generate_enum_members(enum_members, aliases):
    return '\n'.join(map(
        lambda member: '\n'.join(itertools.chain(
            ('            {} = {},'.format(member.displayname, member.enum_value),),
            ('            {} = {},'.format(alias_name, member.displayname)
                for alias_name in aliases.get(member.displayname, [])))),
        enum_members))

def _generate_enum_decls(enum_decls, alias_info):
    return '\n\n'.join(map(
            lambda (name, enum): (
"""\
        enum {name}
        {{
{members}
        }};\
""".format(name=name, members=_generate_enum_members(
    enum.get_children(), alias_info.get(name, {})))),
            enum_decls.items()))

def _generate_constant_decls(macro_defs, macro_type_map):
    constants = '\n'.join(map(
        lambda (name, macro): '            static constexpr {type} const {name} = {value};'.format(
            type=macro_type_map[name], name=name, value=list(macro.get_tokens())[1].spelling),
        filter(lambda (name, _): name in macro_type_map, macro_defs.items())))
    if 'OFP_NO_BUFFER' not in macro_defs:
        return '\n'.join([constants, '            static constexpr std::uint32_t const OFP_NO_BUFFER = 0xffffffff;'])
    return constants

def _generate_constant_defs(macro_defs, macro_type_map):
    constants = '\n'.join(map(
        lambda (name, macro): '        template <class T> {type} const static_data_member_initializer<T>::{name};'.format(
            type=macro_type_map[name], name=name),
        filter(lambda (name, _): name in macro_type_map, macro_defs.items())))
    if 'OFP_NO_BUFFER' not in macro_defs:
        return '\n'.join([constants, '        template <class T> std::uint32_t const static_data_member_initializer<T>::OFP_NO_BUFFER;'])
    return constants

_alias_enumerations = {
        10: {
            'ofp_port': {
                    'OFPP_NONE': ('OFPP_ANY',),
            },
            'ofp_type': {
                    'OFPT_VENDOR': ('OFPT_EXPERIMENTER',),
            },
            'ofp_action_type': {
                    'OFPAT_STRIP_VLAN': ('OFPAT_POP_VLAN',),
                    'OFPAT_ENQUEUE': ('OFPAT_SET_QUEUE',),
                    'OFPAT_VENDOR': ('OFPAT_EXPERIMENTER',),
            },
            'ofp_flow_wildcards': {
                    'OFPFW_IN_PORT': ('OFPXMT_OFB_IN_PORT',),
                    'OFPFW_DL_VLAN': ('OFPXMT_OFB_VLAN_VID',),
                    'OFPFW_DL_SRC': ('OFPXMT_OFB_ETH_SRC',),
                    'OFPFW_DL_DST': ('OFPXMT_OFB_ETH_DST',),
                    'OFPFW_DL_TYPE': ('OFPXMT_OFB_ETH_TYPE',),
                    'OFPFW_NW_PROTO': ('OFPXMT_OFB_IP_PROTO', 'OFPXMT_OFB_ARP_OP'),
                    'OFPFW_TP_SRC': ('OFPXMT_OFB_TCP_SRC', 'OFPXMT_OFB_UDP_SRC', 'OFPXMT_OFB_ICMPV4_TYPE'),
                    'OFPFW_TP_DST': ('OFPXMT_OFB_TCP_DST', 'OFPXMT_OFB_UDP_DST', 'OFPXMT_OFB_ICMPV4_CODE'),
                    'OFPFW_NW_SRC_ALL': ('OFPXMT_OFB_IPV4_SRC', 'OFPXMT_OFB_ARP_SPA'),
                    'OFPFW_NW_DST_ALL': ('OFPXMT_OFB_IPV4_DST', 'OFPXMT_OFB_ARP_TPA'),
                    'OFPFW_DL_VLAN_PCP': ('OFPXMT_OFB_VLAN_PCP',),
                    'OFPFW_NW_TOS': ('OFPXMT_OFB_IP_DSCP',),
            },
            'ofp_stats_types': {
                    'OFPST_DESC': ('OFPMP_DESC',),
                    'OFPST_FLOW': ('OFPMP_FLOW',),
                    'OFPST_AGGREGATE': ('OFPMP_AGGREGATE',),
                    'OFPST_TABLE': ('OFPMP_TABLE',),
                    'OFPST_PORT': ('OFPMP_PORT_STATS',),
                    'OFPST_QUEUE': ('OFPMP_QUEUE',),
                    'OFPST_VENDOR': ('OFPMP_EXPERIMENTER',),
            },
            'ofp_stats_reply_flags': {
                    'OFPSF_REPLY_MORE': ('OFPMPF_REPLY_MORE',),
            },
        },
        13: {},
}


def generate(collector, macro_type_map):
    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_OPENFLOW_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_OPENFLOW_HPP

#include <cstddef>
#include <cstdint>

namespace canard {{
namespace network {{
namespace openflow {{
namespace v{version} {{

    namespace v{version}_detail {{

{struct_decls}

        template <class T>
        class static_data_member_initializer
        {{
        public:
{constant_decls}
        }};

{constant_defs}

    }} // namespace v{version}_detail

    class protocol
        : public v{version}_detail::static_data_member_initializer<protocol>
    {{
    public:
{enum_decls}
    }};

}} // namespace v{version}
}} // namespace openflow
}} // namespace network
}} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V{version}_OPENFLOW_HPP\
""".format(
    version=collector.version,
    struct_decls=_generate_struct_decls(collector.struct_decls, collector.version == 13),
    constant_decls=_generate_constant_decls(collector.macro_defs, macro_type_map),
    constant_defs=_generate_constant_defs(collector.macro_defs, macro_type_map),
    enum_decls=_generate_enum_decls(collector.enum_decls, _alias_enumerations[collector.version])))

