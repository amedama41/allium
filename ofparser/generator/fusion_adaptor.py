from future_builtins import map, filter

def _maybe_qualify(version, typename):
    if typename.startswith('uint'):
        return 'std::' + typename
    if typename.startswith('ofp_'):
        return 'canard::network::openflow::v{version}::detail::{name}'.format(version=version, name=typename)
    return typename

def _enumerate_array(version, array):
    return '\n'.join(map(
        lambda i: '    ({type}, {name}[{index}])'.format(
            type=_maybe_qualify(version, array.element_type), name=array.name, index=i),
        xrange(array.element_count)))

def _generate_member_ppseq(version, members, is_skip_ofp_match):
    def to_type_and_name_tuple(mem):
        if mem.is_array():
            return _enumerate_array(version, mem)
        return '    ({type}, {name})'.format(
                type=_maybe_qualify(version, mem.type), name=mem.name)

    return '\n'.join(map(
        to_type_and_name_tuple,
        filter(
            lambda mem: not (
                (is_skip_ofp_match and mem.type == 'ofp_match')
                or (mem.is_array() and (mem.element_count == 0 or mem.element_type == 'char'))),
            members)))

def _generate_adapt_structs(collector):
    return '\n\n'.join(map(
        lambda (name, struct): (
"""\
BOOST_FUSION_ADAPT_STRUCT(
    canard::network::openflow::v{version}::detail::{name},
{member_ppseq}
)\
""".format(version=collector.version, name=name, member_ppseq=_generate_member_ppseq(
    collector.version, struct.members, is_skip_ofp_match=(collector.version == 13)))),
        collector.struct_decls.items()))

def generate(collector):
    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_FUSION_ADATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_FUSION_ADATOR_HPP

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <canard/network/protocol/openflow/v{version}/openflow.hpp>

{adapt_structs}

#endif // CANARD_NETWORK_OPENFLOW_V{version}_FUSION_ADATOR_HPP\
""".format(version=collector.version, adapt_structs=_generate_adapt_structs(collector)))

