from future_builtins import map

def _generate_byteorder_funcs(funcname, functorname, struct_decls):
    return '\n\n'.join(map(
        lambda (name, _): (
"""\
        inline auto {funcname}({structname} data)
            -> {structname}
        {{
            boost::fusion::for_each(data, {functorname}{{}});
            return data;
        }}\
""".format(funcname=funcname, functorname=functorname, structname=name)),
        struct_decls.items()))

def generate(collector):
    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <canard/byteorder.hpp>
#include <canard/network/protocol/openflow/v{version}/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v{version}/openflow.hpp>

namespace canard {{
namespace network {{
namespace openflow {{
namespace v{version} {{

    namespace detail {{

        using canard::hton;
        using canard::ntoh;

        struct host_to_network
        {{
            template <class Field>
            void operator()(Field& field) const
            {{
                field = hton(field);
            }}
        }};

        struct network_to_host
        {{
            template <class Field>
            void operator()(Field& field) const
            {{
                field = ntoh(field);
            }}
        }};

{hton_funcs}

{ntoh_funcs}

    }} // namespace detail

}} // namespace v{version}
}} // namespace openflow
}} // namespace network
}} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP\
""".format(
        version=collector.version,
        hton_funcs=_generate_byteorder_funcs('hton', 'host_to_network', collector.struct_decls),
        ntoh_funcs=_generate_byteorder_funcs('ntoh', 'network_to_host', collector.struct_decls)))

