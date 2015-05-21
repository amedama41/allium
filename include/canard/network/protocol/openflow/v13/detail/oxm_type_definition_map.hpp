#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_DEFINITION_MAP_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_DEFINITION_MAP_HPP

#include <cstddef>
#include <cstdint>
#include <array>
#include <type_traits>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <ofp_oxm_class OXMClass, std::uint8_t OXMField>
        struct oxm_type;

        using oxm_type_definition_map = boost::fusion::map<
              boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT>,        boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PHY_PORT>,    boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_METADATA>,       boost::fusion::vector<std::uint64_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST>,        boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC>,        boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE>,       boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID>,       boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP>,       boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_DSCP>,        boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_ECN>,         boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_PROTO>,       boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC>,       boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST>,       boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC>,        boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST>,        boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_SRC>,        boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_DST>,        boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_SCTP_SRC>,       boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_SCTP_DST>,       boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV4_TYPE>,    boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV4_CODE>,    boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_OP>,         boost::fusion::vector<std::uint16_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_SPA>,        boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_TPA>,        boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_SHA>,        boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_THA>,        boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_SRC>,       boost::fusion::vector<std::array<std::uint8_t, 16>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_DST>,       boost::fusion::vector<std::array<std::uint8_t, 16>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_FLABEL>,    boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV6_TYPE>,    boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV6_CODE>,    boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_ND_TARGET>, boost::fusion::vector<std::array<std::uint8_t, 16>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_ND_SLL>,    boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_ND_TLL>,    boost::fusion::vector<std::array<std::uint8_t, 6>, std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL>,     boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_TC>,        boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_BOS>,       boost::fusion::vector<std::uint8_t,  std::false_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PBB_ISID>,       boost::fusion::vector<std::uint32_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TUNNEL_ID>,      boost::fusion::vector<std::uint64_t, std::true_type>>
            , boost::fusion::pair<oxm_type<OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV6_EXTHDR>,    boost::fusion::vector<std::uint16_t, std::true_type>>
        >;

        class oxm_match;
        template <ofp_oxm_class OXMClass, std::uint8_t OXMField>
        class basic_oxm_type
        {
            template <std::size_t M>
            using oxm_type_definition = boost::fusion::result_of::value_at_c<
                      typename boost::fusion::result_of::value_at_key<
                              oxm_type_definition_map
                            , v13_detail::oxm_type<OXMClass, OXMField>
                      >::type
                    , M
            >;

        public:
            using value_type = typename oxm_type_definition<0>::type;
            using needs_byteorder_conversion = typename oxm_type_definition<1>::type;

            static constexpr auto oxm_class()
                -> ofp_oxm_class
            {
                return OXMClass;
            }

            static constexpr auto oxm_field()
                -> std::uint8_t
            {
                return OXMField;
            }

            static constexpr auto oxm_type()
                -> std::uint32_t
            {
                return std::uint32_t{OXMClass} << 7 | OXMField;
            }

            auto check_prerequisite(oxm_match const& match) const
                -> bool
            {
                return true;
            }
        };

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_DEFINITION_MAP_HPP
