from future_builtins import map

def _generate_byteorder_funcs(struct_decls):
    return '\n\n'.join(map(
        lambda (name, _): (
"""\
        inline void endian_reverse_inplace({structname}& data) noexcept
        {{
            boost::fusion::for_each(data, generic_endian_reverse_inplace{{}});
        }}

        inline auto endian_reverse({structname} data) noexcept
            -> {structname}
        {{
            endian_reverse_inplace(data);
            return data;
        }}\
""".format(structname=name)),
        struct_decls.items()))

def generate(collector):
    return (
"""\
#ifndef CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP
#define CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP

#include <boost/endian/conversion.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <canard/network/protocol/openflow/v{version}/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v{version}/openflow.hpp>

namespace canard {{
namespace network {{
namespace openflow {{
namespace v{version} {{

    namespace v{version}_detail {{

        struct generic_endian_reverse_inplace
        {{
            template <class Field>
            void operator()(Field& field) const
            {{
                using boost::endian::endian_reverse_inplace;
                endian_reverse_inplace(field);
            }}
        }};

{endian_reverse_funcs}

    }} // namespace v{version}_detail

}} // namespace v{version}
}} // namespace openflow
}} // namespace network
}} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V{version}_BYTEORDER_CONVERSION_HPP\
""".format(
        version=collector.version,
        endian_reverse_funcs=_generate_byteorder_funcs(collector.struct_decls)))

