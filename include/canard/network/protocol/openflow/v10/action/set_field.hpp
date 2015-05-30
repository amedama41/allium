#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_SET_VLAN_VID_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_SET_VLAN_VID_HPP

#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <boost/fusion/algorithm/query/find_if.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/mpl/placeholders.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/network/protocol/openflow/v10/detail/action_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/match_fields.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    namespace set_field_detail {

        template <ofp_action_type ActionType>
        using action_type = std::integral_constant<ofp_action_type, ActionType>;

        using set_field_table = std::tuple<
          //  ofp_match field type         action_type                       set_field type
          //+----------------------------+---------------------------------+---------------------------------+
              std::tuple<match::eth_src  , action_type<OFPAT_SET_DL_SRC>   , v10_detail::ofp_action_dl_addr  >
            , std::tuple<match::eth_dst  , action_type<OFPAT_SET_DL_DST>   , v10_detail::ofp_action_dl_addr  >
            , std::tuple<match::vlan_vid , action_type<OFPAT_SET_VLAN_VID> , v10_detail::ofp_action_vlan_vid >
            , std::tuple<match::vlan_pcp , action_type<OFPAT_SET_VLAN_PCP> , v10_detail::ofp_action_vlan_pcp >
            , std::tuple<match::ipv4_tos , action_type<OFPAT_SET_NW_TOS>   , v10_detail::ofp_action_nw_tos   >
            , std::tuple<match::ipv4_src , action_type<OFPAT_SET_NW_SRC>   , v10_detail::ofp_action_nw_addr  >
            , std::tuple<match::ipv4_dst , action_type<OFPAT_SET_NW_DST>   , v10_detail::ofp_action_nw_addr  >
            , std::tuple<match::tcp_src  , action_type<OFPAT_SET_TP_SRC>   , v10_detail::ofp_action_tp_port  >
            , std::tuple<match::tcp_dst  , action_type<OFPAT_SET_TP_DST>   , v10_detail::ofp_action_tp_port  >
          //+----------------------------+---------------------------------+---------------------------------+
        >;

        template <class MatchField>
        struct set_field_info
        {
            template <class T>
            struct is_same_match
                : std::is_same<typename std::tuple_element<0, T>::type, MatchField>
            {
            };

            using info_tuple = typename std::remove_reference<
                typename boost::fusion::result_of::deref<
                    typename boost::fusion::result_of::find_if<
                        set_field_table, is_same_match<boost::mpl::_1>
                    >::type
                >::type
            >::type;

            static ofp_action_type const value
                = std::tuple_element<1, info_tuple>::type::value;
            using type = typename std::tuple_element<2, info_tuple>::type;
        };

        template <class ValueType, class SetFieldInfo>
        auto to_ofp_action(ValueType value, SetFieldInfo)
            -> typename SetFieldInfo::type
        {
            using ofp_action_t = typename SetFieldInfo::type;
            return ofp_action_t{
                SetFieldInfo::value, sizeof(ofp_action_t), value
            };
        }

        template <class SetFieldInfo>
        auto to_ofp_action(boost::asio::ip::address_v4 const& value, SetFieldInfo)
            -> typename SetFieldInfo::type
        {
            using ofp_action_t = typename SetFieldInfo::type;
            return ofp_action_t{
                SetFieldInfo::value, sizeof(ofp_action_t), value.to_ulong()
            };
        }

        template <class SetFieldInfo>
        auto to_ofp_action(canard::mac_address const& value, SetFieldInfo)
            -> typename SetFieldInfo::type
        {
            using ofp_action_t = typename SetFieldInfo::type;
            auto const& bytes = value.to_bytes();
            return ofp_action_t{
                  SetFieldInfo::value, sizeof(ofp_action_t)
                , {bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]}
            };
        }

        template <class OFPAction>
        auto access(OFPAction const& action_set_field)
            -> typename boost::fusion::result_of::at_c<OFPAction, 2>::type
        {
            return boost::fusion::at_c<2>(action_set_field);
        }

        auto access(v10_detail::ofp_action_dl_addr const& set_dl_addr)
            -> canard::mac_address
        {
            return canard::mac_address{set_dl_addr.dl_addr};
        }

    } // namespace set_field_detail


    template <class MatchField>
    class set_field
        : public v10_detail::action_adaptor<
              set_field<MatchField>
            , typename set_field_detail::set_field_info<MatchField>::type
          >
    {
        using set_field_info
            = set_field_detail::set_field_info<MatchField>;
        using ofp_action_t = typename set_field_info::type;
        using match_field_t = MatchField;
        using value_type = typename match_field_t::value_type;

    public:
        static ofp_action_type const action_type = set_field_info::value;

        explicit set_field(value_type const& value)
            : set_field_(set_field_detail::to_ofp_action(value, set_field_info{}))
        {
        }

        explicit set_field(match_field_t const& field)
            : set_field{field.value()}
        {
        }

        auto value() const
            -> value_type
        {
            return set_field_detail::access(set_field_);
        };

        auto match_field() const
            -> match_field_t
        {
            return match_field_t{value()};
        }

    private:
        friend v10_detail::action_adaptor<set_field<MatchField>, ofp_action_t>;

        auto ofp_action() const
            -> ofp_action_t const&
        {
            return set_field_;
        }

        explicit set_field(ofp_action_t const& action)
            : set_field_(action)
        {
        }

    private:
        ofp_action_t set_field_;
    };

    using set_eth_src = set_field<match::eth_src>;
    using set_eth_dst = set_field<match::eth_dst>;
    using set_vlan_vid = set_field<match::vlan_vid>;
    using set_vlan_pcp = set_field<match::vlan_pcp>;
    using set_ipv4_tos = set_field<match::ipv4_tos>;
    using set_ipv4_src = set_field<match::ipv4_src>;
    using set_ipv4_dst = set_field<match::ipv4_dst>;
    using set_tcp_dst = set_field<match::tcp_dst>;
    using set_tcp_src = set_field<match::tcp_src>;
    using set_udp_dst = set_field<match::udp_dst>;
    using set_udp_src = set_field<match::udp_src>;

} // namespace actions
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_SET_VLAN_VID_HPP
