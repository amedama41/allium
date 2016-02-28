#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_SET_VLAN_VID_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_SET_VLAN_VID_HPP

#include <type_traits>
#include <utility>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/support/pair.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_action.hpp>
#include <canard/network/protocol/openflow/v10/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/match_fields.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions {

    namespace set_field_detail {

        template <protocol::ofp_action_type ActionType, class OFPAction>
        struct ofp_action_info
        {
            static constexpr protocol::ofp_action_type action_type = ActionType;
            using raw_ofp_type = OFPAction;
        };

        using set_field_info_table = boost::fusion::map<
              boost::fusion::pair<match::eth_src  , ofp_action_info<protocol::OFPAT_SET_DL_SRC   , v10_detail::ofp_action_dl_addr  > >
            , boost::fusion::pair<match::eth_dst  , ofp_action_info<protocol::OFPAT_SET_DL_DST   , v10_detail::ofp_action_dl_addr  > >
            , boost::fusion::pair<match::vlan_vid , ofp_action_info<protocol::OFPAT_SET_VLAN_VID , v10_detail::ofp_action_vlan_vid > >
            , boost::fusion::pair<match::vlan_pcp , ofp_action_info<protocol::OFPAT_SET_VLAN_PCP , v10_detail::ofp_action_vlan_pcp > >
            , boost::fusion::pair<match::ip_dscp  , ofp_action_info<protocol::OFPAT_SET_NW_TOS   , v10_detail::ofp_action_nw_tos   > >
            , boost::fusion::pair<match::ipv4_src , ofp_action_info<protocol::OFPAT_SET_NW_SRC   , v10_detail::ofp_action_nw_addr  > >
            , boost::fusion::pair<match::ipv4_dst , ofp_action_info<protocol::OFPAT_SET_NW_DST   , v10_detail::ofp_action_nw_addr  > >
            , boost::fusion::pair<match::tcp_src  , ofp_action_info<protocol::OFPAT_SET_TP_SRC   , v10_detail::ofp_action_tp_port  > >
            , boost::fusion::pair<match::tcp_dst  , ofp_action_info<protocol::OFPAT_SET_TP_DST   , v10_detail::ofp_action_tp_port  > >
        >;

        template <class MatchField>
        struct set_field_info
        {
            using ofp_action_info
                = typename boost::fusion::result_of::value_at_key<
                        set_field_info_table, MatchField
                  >::type;

            static constexpr protocol::ofp_action_type action_type
                = ofp_action_info::action_type;

            using raw_ofp_type = typename ofp_action_info::raw_ofp_type;
        };

        template <class ValueType, class SetFieldInfo>
        auto to_ofp_action(ValueType value, SetFieldInfo) noexcept
            -> typename SetFieldInfo::raw_ofp_type
        {
            using raw_ofp_type = typename SetFieldInfo::raw_ofp_type;
            return raw_ofp_type{
                SetFieldInfo::action_type, sizeof(raw_ofp_type), value
            };
        }

        template <class SetFieldInfo>
        auto to_ofp_action(
                boost::asio::ip::address_v4 const& value, SetFieldInfo) noexcept
            -> typename SetFieldInfo::raw_ofp_type
        {
            using raw_ofp_type = typename SetFieldInfo::raw_ofp_type;
            return raw_ofp_type{
                  SetFieldInfo::action_type
                , sizeof(raw_ofp_type)
                , std::uint32_t(value.to_ulong())
            };
        }

        template <class SetFieldInfo>
        auto to_ofp_action(
                canard::mac_address const& value, SetFieldInfo) noexcept
            -> typename SetFieldInfo::raw_ofp_type
        {
            using raw_ofp_type = typename SetFieldInfo::raw_ofp_type;
            auto const& bytes = value.to_bytes();
            return raw_ofp_type{
                  SetFieldInfo::action_type
                , sizeof(raw_ofp_type)
                , { bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5] }
                , { 0, 0, 0, 0, 0, 0 }
            };
        }

        template <class OFPAction>
        auto access(OFPAction const& action_set_field) noexcept
            -> typename boost::fusion::result_of::at_c<OFPAction const, 2>::type
        {
            return boost::fusion::at_c<2>(action_set_field);
        }

        auto access(v10_detail::ofp_action_dl_addr const& set_dl_addr) noexcept
            -> canard::mac_address
        {
            return canard::mac_address{set_dl_addr.dl_addr};
        }

        auto access(v10_detail::ofp_action_nw_addr const& set_nw_addr)
            -> boost::asio::ip::address_v4
        {
            return boost::asio::ip::address_v4{set_nw_addr.nw_addr};
        }

    } // namespace set_field_detail


    template <
          class MatchField
        , class SetFieldInfo = set_field_detail::set_field_info<MatchField>
    >
    class set_field
        : public actions_detail::basic_action<
                set_field<MatchField>, typename SetFieldInfo::raw_ofp_type
          >
    {
        using raw_ofp_type = typename SetFieldInfo::raw_ofp_type;
        using value_type = typename MatchField::value_type;

    public:
        static constexpr protocol::ofp_action_type action_type
            = SetFieldInfo::action_type;

        explicit set_field(value_type const& value) noexcept
            : set_field_(set_field_detail::to_ofp_action(value, SetFieldInfo{}))
        {
        }

        explicit set_field(MatchField const& field) noexcept
            : set_field{field.value()}
        {
        }

        auto value() const noexcept
            -> value_type
        {
            return set_field_detail::access(set_field_);
        };

        auto match_field() const noexcept
            -> MatchField
        {
            return MatchField{value()};
        }

        template <class Action>
        static auto validate(Action&& action)
            -> typename std::enable_if<
                  is_same_value_type<Action, set_field>::value, Action&&
               >::type
        {
            match_detail::validate(
                    action.value(), typename MatchField::field_type{});
            return std::forward<Action>(action);
        }

    private:
        friend actions_detail::basic_action<set_field<MatchField>, raw_ofp_type>;

        auto ofp_action() const noexcept
            -> raw_ofp_type const&
        {
            return set_field_;
        }

        explicit set_field(raw_ofp_type const& action) noexcept
            : set_field_(action)
        {
        }

    private:
        raw_ofp_type set_field_;
    };

    template <class MatchField>
    auto operator==(
              set_field<MatchField> const& lhs
            , set_field<MatchField> const& rhs) noexcept
        -> bool
    {
        return lhs.value() == rhs.value();
    }

    using set_eth_src = set_field<match::eth_src>;
    using set_eth_dst = set_field<match::eth_dst>;
    using set_vlan_vid = set_field<match::vlan_vid>;
    using set_vlan_pcp = set_field<match::vlan_pcp>;
    using set_ip_dscp = set_field<match::ip_dscp>;
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
