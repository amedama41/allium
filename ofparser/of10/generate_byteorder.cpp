#include <fstream>
#include <vector>
#include <boost/format.hpp>
#include "code_information.hpp"

void generate_byteorder(std::string const& version, std::vector<struct_info> const& sinfos)
{
    auto format = boost::format{
R"(
        inline auto %1%(%2% data)
            -> %2%
        {
            boost::fusion::for_each(data, %3%{});
            return data;
        }
)"
    };

    std::ofstream ofs{
        (boost::format{"../../include/canard/network/protocol/openflow/v%1%/detail/byteorder.hpp"}
         % version).str()
    };
    ofs << boost::format{
R"(#ifndef CANARD_NETWORK_OPENFLOW_V%1%_BYTEORDER_CONVERSION_HPP
#define CANARD_NETWORK_OPENFLOW_V%1%_BYTEORDER_CONVERSION_HPP

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <canard/byteorder.hpp>
#include <canard/network/protocol/openflow/v%1%/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v%1%/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v%1% {

    namespace detail {

        using canard::hton;
        using canard::ntoh;

        struct host_to_network
        {
            template <class Field>
            void operator()(Field& field) const
            {
                field = hton(field);
            }
        };

        struct network_to_host
        {
            template <class Field>
            void operator()(Field& field) const
            {
                field = ntoh(field);
            }
        };

)"
    } % version;
    for (auto& info : sinfos) {
        ofs << format % "hton" % info.name % "host_to_network";
    }
    ofs << std::endl;
    for (auto& info : sinfos) {
        ofs << format % "ntoh" % info.name % "network_to_host";
    }
    ofs << boost::format{
R"(
    } // namespace detail

} // namespace v%1%
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V%1%_BYTEORDER_CONVERSION_HPP
)"
    } % version;
}

