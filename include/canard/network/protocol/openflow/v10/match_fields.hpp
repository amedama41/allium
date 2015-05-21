#ifndef CANARD_NETWORK_OPENFLOW_V10_MATCH_FIELDS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MATCH_FIELDS_HPP

#include <cstdint>
#include <cstring>
#include <array>
#include <type_traits>
#include <utility>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/mpl/int.hpp>
#include <canard/network/protocol/openflow/v10/detail/fusion_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    namespace match_field_detail {

        template <std::uint32_t Field>
        using field_type = std::integral_constant<std::uint32_t, Field>;

        using match_field_table = boost::fusion::map<
          //  ofp_match field type                                ofp_match member index
          //+---------------------------------------------------+------------------------+
              boost::fusion::pair<field_type<OFPFW_IN_PORT>     , boost::mpl::int_<1>>
            , boost::fusion::pair<field_type<OFPFW_DL_SRC>      , boost::mpl::int_<2>>
            , boost::fusion::pair<field_type<OFPFW_DL_DST>      , boost::mpl::int_<8>>
            , boost::fusion::pair<field_type<OFPFW_DL_VLAN>     , boost::mpl::int_<14>>
            , boost::fusion::pair<field_type<OFPFW_DL_VLAN_PCP> , boost::mpl::int_<15>>
            , boost::fusion::pair<field_type<OFPFW_DL_TYPE>     , boost::mpl::int_<17>>
            , boost::fusion::pair<field_type<OFPFW_NW_TOS>      , boost::mpl::int_<18>>
            , boost::fusion::pair<field_type<OFPFW_NW_PROTO>    , boost::mpl::int_<19>>
            , boost::fusion::pair<field_type<OFPFW_NW_SRC_ALL>  , boost::mpl::int_<22>>
            , boost::fusion::pair<field_type<OFPFW_NW_DST_ALL>  , boost::mpl::int_<23>>
            , boost::fusion::pair<field_type<OFPFW_TP_SRC>      , boost::mpl::int_<24>>
            , boost::fusion::pair<field_type<OFPFW_TP_DST>      , boost::mpl::int_<25>>
          //+---------------------------------------------------+------------------------+
        >;

        template <class FieldType>
        struct get_field_value_type
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;

            using type = typename boost::fusion::result_of::value_at<
                v10_detail::ofp_match, member_index
            >::type;
        };

        template <>
        struct get_field_value_type<field_type<OFPFW_DL_DST>>
        {
            using type = std::array<std::uint8_t, 6>;
        };

        template <>
        struct get_field_value_type<field_type<OFPFW_DL_SRC>>
        {
            using type = std::array<std::uint8_t, 6>;
        };

        template <class FieldType>
        class match_field
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;

            using field_value_type = typename get_field_value_type<FieldType>::type;

        public:
            explicit match_field(field_value_type const& value)
                : value_(value)
            {
            }

            template <class T, typename std::enable_if<!std::is_scalar<T>::value>::type* = nullptr>
            explicit match_field(T&& t)
                : value_(std::forward<T>(t).to_bytes())
            {
            }

            void set_value(v10_detail::ofp_match& match) const
            {
                boost::fusion::at<member_index>(match) = value_;
            }

            auto wildcards() const
                -> std::uint32_t
            {
                return ~FieldType::value;
            }

        private:
            field_value_type value_;
        };

        template <>
        inline void match_field<field_type<OFPFW_DL_DST>>::set_value(v10_detail::ofp_match& match) const
        {
            std::memcpy(&boost::fusion::at<member_index>(match), value_.data(), value_.size());
        }

        template <>
        inline void match_field<field_type<OFPFW_DL_SRC>>::set_value(v10_detail::ofp_match& match) const
        {
            std::memcpy(&boost::fusion::at<member_index>(match), value_.data(), value_.size());
        }

        template <class FieldType>
        struct mask_info;

        template <>
        struct mask_info<field_type<OFPFW_NW_SRC_ALL>>
        {
            static std::uint32_t const shift = OFPFW_NW_SRC_SHIFT;
            static std::uint32_t const mask = OFPFW_NW_SRC_MASK;
        };

        template <>
        struct mask_info<field_type<OFPFW_NW_DST_ALL>>
        {
            static std::uint32_t const shift = OFPFW_NW_DST_SHIFT;
            static std::uint32_t const mask = OFPFW_NW_DST_MASK;
        };

        template <class FieldType>
        class ipaddr_match_field_base
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;

            using field_value_type = typename boost::fusion::result_of::value_at<
                v10_detail::ofp_match, member_index
            >::type;

        public:
            ipaddr_match_field_base(field_value_type const& value)
                : value_(value)
                , cidr_suffix_(32)
            {
            }

            ipaddr_match_field_base(field_value_type const& value, std::uint8_t const cidr_suffix)
                : value_(value)
                , cidr_suffix_(cidr_suffix)
            {
            }

            void set_value(v10_detail::ofp_match& match) const
            {
                boost::fusion::at<member_index>(match) = value_;
            }

            auto wildcards() const
                -> std::uint32_t
            {
                return ~mask_info<FieldType>::mask | ((32 - cidr_suffix_) << mask_info<FieldType>::shift);
            }

        private:
            field_value_type value_;
            std::uint32_t cidr_suffix_;
        };

        template <>
        class match_field<field_type<OFPFW_NW_SRC_ALL>>
            : public ipaddr_match_field_base<field_type<OFPFW_NW_SRC_ALL>>
        {
            using ipaddr_match_field_base::ipaddr_match_field_base;
        };

        template <>
        class match_field<field_type<OFPFW_NW_DST_ALL>>
            : public ipaddr_match_field_base<field_type<OFPFW_NW_DST_ALL>>
        {
            using ipaddr_match_field_base::ipaddr_match_field_base;
        };

    } // namespace match_field_detail

    namespace match {

        using in_port = match_field_detail::match_field<match_field_detail::field_type<OFPFW_IN_PORT>>;
        using eth_src = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_SRC>>;
        using eth_dst = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_DST>>;
        using vlan_vid = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_VLAN>>;
        using vlan_pcp = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_VLAN_PCP>>;
        using eth_type = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_TYPE>>;
        using ipv4_tos = match_field_detail::match_field<match_field_detail::field_type<OFPFW_DL_TYPE>>;
        using ip_proto = match_field_detail::match_field<match_field_detail::field_type<OFPFW_NW_PROTO>>;
        using ipv4_src = match_field_detail::match_field<match_field_detail::field_type<OFPFW_NW_SRC_ALL>>;
        using ipv4_dst = match_field_detail::match_field<match_field_detail::field_type<OFPFW_NW_DST_ALL>>;
        using arp_spa = match_field_detail::match_field<match_field_detail::field_type<OFPFW_NW_SRC_ALL>>;
        using arp_tpa = match_field_detail::match_field<match_field_detail::field_type<OFPFW_NW_DST_ALL>>;
        using tcp_src = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_SRC>>;
        using tcp_dst = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_DST>>;
        using udp_src = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_SRC>>;
        using udp_dst = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_DST>>;
        using icmpv4_type = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_SRC>>;
        using icmpv4_code = match_field_detail::match_field<match_field_detail::field_type<OFPFW_TP_DST>>;

    } // namespace match

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MATCH_FIELDS_HPP
