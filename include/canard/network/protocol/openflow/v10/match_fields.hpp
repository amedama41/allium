#ifndef CANARD_NETWORK_OPENFLOW_V10_MATCH_FIELDS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MATCH_FIELDS_HPP

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/mpl/int.hpp>
#include <canard/mac_address.hpp>
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


        template <class FieldType, class Enable = void>
        struct field_value_type
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;

            using type = typename boost::fusion::result_of::value_at<
                v10_detail::ofp_match, member_index
            >::type;
        };

        template <class FieldType>
        struct field_value_type<
                  FieldType
                , typename std::enable_if<
                           FieldType::value == OFPFW_DL_SRC
                        || FieldType::value == OFPFW_DL_DST
                  >::type>
        {
            using type = canard::mac_address;
        };

        template <class FieldType>
        struct field_value_type<
                  FieldType
                , typename std::enable_if<
                           FieldType::value == OFPFW_NW_SRC_ALL
                        || FieldType::value == OFPFW_NW_DST_ALL
                  >::type>
        {
            using type = boost::asio::ip::address_v4;
        };


        template <
              class FieldType
            , typename std::enable_if<
                       FieldType::value != OFPFW_NW_SRC_ALL
                    && FieldType::value != OFPFW_NW_DST_ALL
              >::type* = nullptr>
        void copy_to_match(
                  v10_detail::ofp_match& match
                , typename field_value_type<FieldType>::type const& value
                , FieldType)
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;
            boost::fusion::at<member_index>(match) = value;
        }

        void copy_to_match(
                  v10_detail::ofp_match& match
                , canard::mac_address const& macaddr
                , field_type<OFPFW_DL_DST>)
        {
            std::memcpy(match.dl_dst, macaddr.to_bytes().data(), macaddr.to_bytes().size());
        }

        void copy_to_match(
                  v10_detail::ofp_match& match
                , canard::mac_address const& macaddr
                , field_type<OFPFW_DL_SRC>)
        {
            std::memcpy(match.dl_src, macaddr.to_bytes().data(), macaddr.to_bytes().size());
        }

        template <
              class FieldType
            , typename std::enable_if<
                       FieldType::value == OFPFW_NW_SRC_ALL
                    || FieldType::value == OFPFW_NW_DST_ALL
              >::type* = nullptr>
        void copy_to_match(
                  v10_detail::ofp_match& match
                , typename field_value_type<FieldType>::type const& value
                , FieldType)
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;
            boost::fusion::at<member_index>(match) = value.to_ulong();
        }


        template <class FieldType>
        auto field_value(v10_detail::ofp_match const& match, FieldType)
            -> typename field_value_type<FieldType>::type
        {
            using member_index = typename boost::fusion::result_of::value_at_key<
                match_field_table, FieldType
            >::type;

            return boost::fusion::at<member_index>(match);
        }

        auto field_value(v10_detail::ofp_match const& match, field_type<OFPFW_DL_DST>)
            -> canard::mac_address
        {
            return canard::mac_address{match.dl_dst};
        }

        auto field_value(v10_detail::ofp_match const& match, field_type<OFPFW_DL_SRC>)
            -> canard::mac_address
        {
            return canard::mac_address{match.dl_src};
        }


        template <class FieldType>
        struct mask_info
        {
            static bool const has_mask = false;
        };

        template <>
        struct mask_info<field_type<OFPFW_NW_SRC_ALL>>
        {
            static bool const has_mask = true;
            static std::uint32_t const shift = OFPFW_NW_SRC_SHIFT;
            static std::uint32_t const mask = OFPFW_NW_SRC_MASK;
        };

        template <>
        struct mask_info<field_type<OFPFW_NW_DST_ALL>>
        {
            static bool const has_mask = true;
            static std::uint32_t const shift = OFPFW_NW_DST_SHIFT;
            static std::uint32_t const mask = OFPFW_NW_DST_MASK;
        };


        template <class FieldType, bool = match_field_detail::mask_info<FieldType>::has_mask>
        class match_field
        {
        public:
            using value_type = typename match_field_detail::field_value_type<FieldType>::type;

            match_field(value_type const& value)
                : value_(value)
            {
            }

            auto value() const
                -> value_type const&
            {
                return value_;
            }

            void set_value(v10_detail::ofp_match& match) const
            {
                match_field_detail::copy_to_match(match, value(), FieldType{});
                match.wildcards &= ~FieldType::value;
            }

            static auto is_wildcard(v10_detail::ofp_match const& match)
                -> bool
            {
                return match.wildcards & FieldType::value;
            }

            static auto from_match(v10_detail::ofp_match const& match)
                -> match_field
            {
                return match_field{match_field_detail::field_value(match, FieldType{})};
            }

        private:
            value_type value_;
        };

        template <class FieldType>
        class match_field<FieldType, true>
        {
            using mask_info = match_field_detail::mask_info<FieldType>;

        public:
            using value_type = typename match_field_detail::field_value_type<FieldType>::type;

            explicit match_field(value_type const value, std::uint8_t const cidr_suffix = 32)
                : value_(value)
                , cidr_suffix_(cidr_suffix)
            {
            }

            auto value() const
                -> value_type const&
            {
                return value_;
            }

            auto cidr_suffix() const
                -> std::uint8_t
            {
                return cidr_suffix_;
            }

            void set_value(v10_detail::ofp_match& match) const
            {
                match_field_detail::copy_to_match(match, value(), FieldType{});
                match.wildcards &= ~mask_info::mask;
                match.wildcards |= (std::uint32_t{32} - cidr_suffix_) << mask_info::shift;
            }

            static auto is_wildcard(v10_detail::ofp_match const& match)
                -> bool
            {
                return cidr_suffix(match) == 0;
            }

            static auto from_match(v10_detail::ofp_match const& match)
                -> match_field
            {
                return match_field{
                      match_field_detail::field_value(match, FieldType{})
                    , cidr_suffix(match)
                };
            }

        private:
            static auto cidr_suffix(v10_detail::ofp_match const& match)
                -> std::uint8_t
            {
                auto const mask_value
                    = (mask_info::mask & match.wildcards) >> mask_info::shift;
                return 32 - std::min<std::uint8_t>(mask_value, 32);
            }

        private:
            value_type value_;
            std::uint8_t cidr_suffix_;
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
