#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/format.hpp>
#include <boost/optional/optional.hpp>
#include <boost/range/algorithm_ext/for_each.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/mac_address.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/oxm_type_definition_map.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace oxm_match {

    namespace oxm_match_detail {

        template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
        struct oxm_type
        {
        };

        template <class OXMValueType, bool NeedsByteOrderConversion = true>
        struct oxm_type_info
        {
            using oxm_value_type = OXMValueType;
            using needs_byte_order_conversion
                = std::integral_constant<bool, NeedsByteOrderConversion>;
        };

        template <std::uint8_t OXMField>
        using basic_oxm_class
            = oxm_type<protocol::OFPXMC_OPENFLOW_BASIC, OXMField>;

        using boost::fusion::pair;

        using oxm_type_info_table = boost::fusion::map<
              pair<basic_oxm_class<protocol::OFPXMT_OFB_IN_PORT>        , oxm_type_info<std::uint32_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IN_PHY_PORT>    , oxm_type_info<std::uint32_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_METADATA>       , oxm_type_info<std::uint64_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ETH_DST>        , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ETH_SRC>        , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ETH_TYPE>       , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_VLAN_VID>       , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_VLAN_PCP>       , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IP_DSCP>        , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IP_ECN>         , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IP_PROTO>       , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV4_SRC>       , oxm_type_info<boost::asio::ip::address_v4>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV4_DST>       , oxm_type_info<boost::asio::ip::address_v4>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_TCP_SRC>        , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_TCP_DST>        , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_UDP_SRC>        , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_UDP_DST>        , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_SCTP_SRC>       , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_SCTP_DST>       , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ICMPV4_TYPE>    , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ICMPV4_CODE>    , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ARP_OP>         , oxm_type_info<std::uint16_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ARP_SPA>        , oxm_type_info<boost::asio::ip::address_v4>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ARP_TPA>        , oxm_type_info<boost::asio::ip::address_v4>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ARP_SHA>        , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ARP_THA>        , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_SRC>       , oxm_type_info<boost::asio::ip::address_v6, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_DST>       , oxm_type_info<boost::asio::ip::address_v6, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_FLABEL>    , oxm_type_info<std::uint32_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ICMPV6_TYPE>    , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_ICMPV6_CODE>    , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_ND_TARGET> , oxm_type_info<boost::asio::ip::address_v6, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_ND_SLL>    , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_ND_TLL>    , oxm_type_info<canard::mac_address, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_MPLS_LABEL>     , oxm_type_info<std::uint32_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_MPLS_TC>        , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_MPLS_BOS>       , oxm_type_info<std::uint8_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_PBB_ISID>       , oxm_type_info<boost::endian::big_uint24_t, false>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_TUNNEL_ID>      , oxm_type_info<std::uint64_t>>
            , pair<basic_oxm_class<protocol::OFPXMT_OFB_IPV6_EXTHDR>    , oxm_type_info<std::uint16_t>>
        >;

        template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
        using oxm_type_info_t = typename boost::fusion::result_of::value_at_key<
              oxm_type_info_table, oxm_type<OXMClass, OXMField>
        >::type;

        template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
        using oxm_value_type
            = typename oxm_type_info_t<OXMClass, OXMField>::oxm_value_type;

        template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
        using needs_byte_order_conversion = typename oxm_type_info_t<
            OXMClass, OXMField
        >::needs_byte_order_conversion;

        template <class T, class OXMTypeInfo>
        void validate_value(T const&, OXMTypeInfo)
        {
        }

        inline void validate_value(
                  std::uint32_t const port_no
                , basic_oxm_class<protocol::OFPXMT_OFB_IN_PORT>)
        {
            if (port_no == 0) {
                throw std::runtime_error{"port no zero is invalid"};
            }
            if (port_no > protocol::OFPP_MAX
                    && port_no != protocol::OFPP_LOCAL
                    && port_no != protocol::OFPP_CONTROLLER) {
                throw std::runtime_error{"invalid port no"};
            }
        }

        inline void validate_value(
                  std::uint32_t const port_no
                , basic_oxm_class<protocol::OFPXMT_OFB_IN_PHY_PORT>)
        {
            if (port_no == 0) {
                throw std::runtime_error{"in physical port no zero is invalid"};
            }
            if (port_no > protocol::OFPP_MAX) {
                throw std::runtime_error{"invalid port no"};
            }
        }

        inline void validate_value(
                  std::uint16_t const vid
                , basic_oxm_class<protocol::OFPXMT_OFB_VLAN_VID>)
        {
            if (vid > (0x0fff | protocol::OFPVID_PRESENT)) {
                throw std::runtime_error{"invalid vlan vid"};
            }
            if (vid != protocol::OFPVID_NONE
                    && !(vid & protocol::OFPVID_PRESENT)) {
                throw std::runtime_error{"no OFPVID_PRESENT"};
            }
        }

        inline void validate_value(
                  std::uint8_t const pcp
                , basic_oxm_class<protocol::OFPXMT_OFB_VLAN_PCP>)
        {
            if (pcp > 0x07) {
                throw std::runtime_error{"invalid vlan pcp"};
            }
        }

        inline void validate_value(
                  std::uint8_t const dscp
                , basic_oxm_class<protocol::OFPXMT_OFB_IP_DSCP>)
        {
            if (dscp > 0x3f) {
                throw std::runtime_error{"invalid ip dscp"};
            }
        }

        inline void validate_value(
                  std::uint8_t const ecn
                , basic_oxm_class<protocol::OFPXMT_OFB_IP_ECN>)
        {
            if (ecn > 0x03) {
                throw std::runtime_error{"invalid ip ecn"};
            }
        }

        inline void validate_value(
                  std::uint32_t const flow_label
                , basic_oxm_class<protocol::OFPXMT_OFB_IPV6_FLABEL>)
        {
            if (flow_label > 0x000fffff) {
                throw std::runtime_error{"invalid ipv6 flow label"};
            }
        }

        inline void validate_value(
                  std::uint32_t const mpls_label
                , basic_oxm_class<protocol::OFPXMT_OFB_MPLS_LABEL>)
        {
            if (mpls_label > 0x000fffff) {
                throw std::runtime_error{"invalid mpls label"};
            }
        }

        inline void validate_value(
                  std::uint8_t const mpls_tc
                , basic_oxm_class<protocol::OFPXMT_OFB_MPLS_TC>)
        {
            if (mpls_tc > 0x07) {
                throw std::runtime_error{"invalid mpls tc"};
            }
        }

        inline void validate_value(
                  std::uint8_t const mpls_bos
                , basic_oxm_class<protocol::OFPXMT_OFB_MPLS_BOS>)
        {
            if (mpls_bos > 0x01) {
                throw std::runtime_error{"invalid mpls bos"};
            }
        }

        inline void validate_value(
                  std::uint16_t const ipv6_exthdr_flags
                , basic_oxm_class<protocol::OFPXMT_OFB_IPV6_EXTHDR>)
        {
            if (ipv6_exthdr_flags > 0x01ff) {
                throw std::runtime_error{"invalid ipv6 extension header flags"};
            }
        }

        template <class T>
        void validate_mask(T const& value, T const& mask)
        {
            boost::for_each(
                      canard::as_byte_range(value)
                    , canard::as_byte_range(mask)
                    , [](std::uint8_t const v, std::uint8_t const m) {
                if (v & ~m) {
                    throw std::runtime_error{"invalid wildcard"};
                }
            });
        }

    } // namespace oxm_match_detail


    template <
          protocol::ofp_oxm_class OXMClass
        , std::uint8_t OXMField
        , class ValueType = oxm_match_detail::oxm_value_type<OXMClass, OXMField>
    >
    class oxm_match_field
        : public detail::v13::basic_oxm_match_field<
            oxm_match_field<OXMClass, OXMField>
          >
    {
    public:
        using value_type = ValueType;

        explicit oxm_match_field(value_type const value) noexcept
            : value_(value)
            , mask_(boost::none)
        {
        }

        oxm_match_field(value_type const value, value_type const mask)
            : value_(value)
            , mask_{mask}
        {
        }

        static constexpr auto oxm_class() noexcept
            -> protocol::ofp_oxm_class
        {
            return OXMClass;
        }

        static constexpr auto oxm_field() noexcept
            -> std::uint8_t
        {
            return OXMField;
        }

        auto oxm_value() const noexcept
            -> value_type
        {
            return value_;
        }

        auto oxm_mask() const noexcept
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        template <class OXMMatchField>
        static auto validate(OXMMatchField&& field)
            -> typename std::enable_if<
                  is_same_value_type<OXMMatchField, oxm_match_field>::value
                , OXMMatchField&&
               >::type
        {
            oxm_match_detail::validate_value(
                      field.oxm_value()
                    , oxm_match_detail::oxm_type<OXMClass, OXMField>{});
            if (field.oxm_has_mask()) {
                oxm_match_detail::validate_mask(
                        field.raw_value(), field.raw_mask());
            }
            return std::forward<OXMMatchField>(field);
        }

    private:
        friend detail::v13::basic_oxm_match_field<oxm_match_field>;

        using oxm_value_type = ValueType;
        using needs_byte_order_conversion
            = oxm_match_detail::needs_byte_order_conversion<OXMClass, OXMField>;

        auto raw_value() const noexcept
            -> oxm_value_type
        {
            return value_;
        }

        auto raw_mask() const noexcept
            -> oxm_value_type
        {
            return *mask_;
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
    class oxm_match_field<OXMClass, OXMField, canard::mac_address>
        : public detail::v13::basic_oxm_match_field<
            oxm_match_field<OXMClass, OXMField, canard::mac_address>
          >
    {
    public:
        using value_type = canard::mac_address;

        explicit oxm_match_field(value_type const& value) noexcept
            : value_(value)
            , mask_(boost::none)
        {
        }

        oxm_match_field(value_type const& value, value_type const& mask)
            : value_(value)
            , mask_{mask}
        {
        }

        static constexpr auto oxm_class() noexcept
            -> protocol::ofp_oxm_class
        {
            return OXMClass;
        }

        static constexpr auto oxm_field() noexcept
            -> std::uint8_t
        {
            return OXMField;
        }

        auto oxm_value() const noexcept
            -> value_type
        {
            return value_;
        }

        auto oxm_mask() const noexcept
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        template <class OXMMatchField>
        static auto validate(OXMMatchField&& field)
            -> typename std::enable_if<
                  is_same_value_type<OXMMatchField, oxm_match_field>::value
                , OXMMatchField&&
               >::type
        {
            if (field.oxm_has_mask()) {
                oxm_match_detail::validate_mask(
                        field.raw_value(), field.raw_mask());
            }
            return std::forward<OXMMatchField>(field);
        }

    private:
        friend detail::v13::basic_oxm_match_field<oxm_match_field>;

        using oxm_value_type = value_type::bytes_type;
        using needs_byte_order_conversion
            = oxm_match_detail::needs_byte_order_conversion<OXMClass, OXMField>;

        explicit oxm_match_field(oxm_value_type const& value)
            : value_{value}
            , mask_(boost::none)
        {
        }

        oxm_match_field(
                oxm_value_type const& value, oxm_value_type const& mask)
            : value_{value}
            , mask_{value_type{mask}}
        {
        }

        auto raw_value() const noexcept
            -> oxm_value_type
        {
            return value_.to_bytes();
        }

        auto raw_mask() const noexcept
            -> oxm_value_type
        {
            return mask_->to_bytes();
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
    class oxm_match_field<OXMClass, OXMField, boost::asio::ip::address_v4>
        : public detail::v13::basic_oxm_match_field<
            oxm_match_field<OXMClass, OXMField, boost::asio::ip::address_v4>
          >
    {
    public:
        using value_type = boost::asio::ip::address_v4;

        explicit oxm_match_field(value_type const& value)
            : value_(value)
            , mask_(boost::none)
        {
        }

        oxm_match_field(value_type const& value, value_type const& mask)
            : value_(value)
            , mask_{mask}
        {
        }

        explicit oxm_match_field(boost::asio::ip::address const& value)
            : oxm_match_field{value.to_v4()}
        {
        }

        oxm_match_field(
                  boost::asio::ip::address const& value
                , boost::asio::ip::address const& mask)
            : oxm_match_field{value.to_v4(), mask.to_v4()}
        {
        }

        oxm_match_field(
                value_type const& value, std::uint8_t const prefix_length)
            : value_(value)
            , mask_{create_ipv4_addr(prefix_length)}
        {
        }

        oxm_match_field(
                  boost::asio::ip::address const& value
                , std::uint8_t const prefix_length)
            : oxm_match_field{value.to_v4(), prefix_length}
        {
        }

        static constexpr auto oxm_class() noexcept
            -> protocol::ofp_oxm_class
        {
            return OXMClass;
        }

        static constexpr auto oxm_field() noexcept
            -> std::uint8_t
        {
            return OXMField;
        }

        auto oxm_value() const noexcept
            -> value_type const&
        {
            return value_;
        }

        auto oxm_mask() const noexcept
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        template <class OXMMatchField>
        static auto validate(OXMMatchField&& field)
            -> typename std::enable_if<
                  is_same_value_type<OXMMatchField, oxm_match_field>::value
                , OXMMatchField&&
               >::type
        {
            if (field.oxm_has_mask()) {
                oxm_match_detail::validate_mask(
                        field.raw_value(), field.raw_mask());
            }
            return std::forward<OXMMatchField>(field);
        }

    private:
        friend detail::v13::basic_oxm_match_field<oxm_match_field>;

        using oxm_value_type = std::uint32_t;
        using needs_byte_order_conversion
            = oxm_match_detail::needs_byte_order_conversion<OXMClass, OXMField>;

        explicit oxm_match_field(oxm_value_type const value)
            : value_{value}
            , mask_(boost::none)
        {
        }

        oxm_match_field(
                oxm_value_type const value, oxm_value_type const mask)
            : value_{value}
            , mask_{value_type{mask}}
        {
        }

        auto raw_value() const noexcept
            -> oxm_value_type
        {
            return value_.to_ulong();
        }

        auto raw_mask() const noexcept
            -> oxm_value_type
        {
            return mask_->to_ulong();
        }

        static auto create_ipv4_addr(std::uint8_t const prefix_length)
            -> boost::asio::ip::address_v4
        {
            return boost::asio::ip::address_v4{
                std::uint32_t(std::uint64_t{0xffffffff} << (32 - prefix_length))
            };
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
    class oxm_match_field<OXMClass, OXMField, boost::asio::ip::address_v6>
        : public detail::v13::basic_oxm_match_field<
            oxm_match_field<OXMClass, OXMField, boost::asio::ip::address_v6>
          >
    {
    public:
        using value_type = boost::asio::ip::address_v6;

        explicit oxm_match_field(value_type const& value)
            : value_(value)
            , mask_(boost::none)
        {
        }

        oxm_match_field(value_type const& value, value_type const& mask)
            : value_(value)
            , mask_{mask}
        {
        }

        explicit oxm_match_field(boost::asio::ip::address const& value)
            : oxm_match_field{value.to_v6()}
        {
        }

        oxm_match_field(
                  boost::asio::ip::address const& value
                , boost::asio::ip::address const& mask)
            : oxm_match_field{value.to_v6(), mask.to_v6()}
        {
        }

        oxm_match_field(
                value_type const& value, std::uint8_t const prefix_length)
            : value_(value)
            , mask_{create_ipv6_addr(prefix_length)}
        {
        }

        oxm_match_field(
                  boost::asio::ip::address const& value
                , std::uint8_t const prefix_length)
            : oxm_match_field{value.to_v6(), prefix_length}
        {
        }

        static constexpr auto oxm_class() noexcept
            -> protocol::ofp_oxm_class
        {
            return OXMClass;
        }

        static constexpr auto oxm_field() noexcept
            -> std::uint8_t
        {
            return OXMField;
        }

        auto oxm_value() const noexcept
            -> value_type const&
        {
            return value_;
        }

        auto oxm_mask() const noexcept
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        template <class OXMMatchField>
        static auto validate(OXMMatchField&& field)
            -> typename std::enable_if<
                  is_same_value_type<OXMMatchField, oxm_match_field>::value
                , OXMMatchField&&
               >::type
        {
            if (field.oxm_has_mask()) {
                oxm_match_detail::validate_mask(
                        field.raw_value(), field.raw_mask());
            }
            return std::forward<OXMMatchField>(field);
        }

    private:
        friend detail::v13::basic_oxm_match_field<oxm_match_field>;

        using oxm_value_type = value_type::bytes_type;
        using needs_byte_order_conversion
            = oxm_match_detail::needs_byte_order_conversion<OXMClass, OXMField>;

        explicit oxm_match_field(oxm_value_type const value)
            : value_{value}
            , mask_(boost::none)
        {
        }

        oxm_match_field(
                oxm_value_type const value, oxm_value_type const mask)
            : value_{value}
            , mask_{value_type{mask}}
        {
        }

        auto raw_value() const noexcept
            -> oxm_value_type
        {
            return value_.to_bytes();
        }

        auto raw_mask() const noexcept
            -> oxm_value_type
        {
            return mask_->to_bytes();
        }

        static auto create_ipv6_addr(std::uint8_t prefix_length)
            -> boost::asio::ip::address_v6
        {
            auto bytes = oxm_value_type{};
            for (std::uint8_t& byte : bytes) {
                if (prefix_length < 8) {
                    byte = (std::uint16_t{0xff} << (8 - prefix_length));
                    break;
                }
                byte = 0xff;
                prefix_length -= 8;
            }
            return boost::asio::ip::address_v6{bytes};
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    using in_port        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT>;
    using in_phy_port    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT>;
    using metadata       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_METADATA>;
    using eth_dst        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST>;
    using eth_src        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC>;
    using eth_type       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE>;
    using vlan_vid       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID>;
    using vlan_pcp       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP>;
    using ip_dscp        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP>;
    using ip_ecn         = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN>;
    using ip_proto       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO>;
    using ipv4_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC>;
    using ipv4_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST>;
    using tcp_src        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC>;
    using tcp_dst        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST>;
    using udp_src        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC>;
    using udp_dst        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST>;
    using sctp_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC>;
    using sctp_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST>;
    using icmpv4_type    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_TYPE>;
    using icmpv4_code    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_CODE>;
    using arp_op         = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_OP>;
    using arp_spa        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SPA>;
    using arp_tpa        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_TPA>;
    using arp_sha        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SHA>;
    using arp_tha        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_THA>;
    using ipv6_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_SRC>;
    using ipv6_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_DST>;
    using ipv6_flabel    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_FLABEL>;
    using icmpv6_type    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_TYPE>;
    using icmpv6_code    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_CODE>;
    using ipv6_nd_target = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TARGET>;
    using ipv6_nd_sll    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_SLL>;
    using ipv6_nd_tll    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TLL>;
    using mpls_label     = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_LABEL>;
    using mpls_tc        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_TC>;
    using mpls_bos       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_BOS>;
    using pbb_isid       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_PBB_ISID>;
    using tunnel_id      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TUNNEL_ID>;
    using ipv6_exthdr    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_EXTHDR>;

} // namespace oxm_match

namespace match {

    template <protocol::ofp_oxm_class OXMClass, std::uint8_t OXMField>
    class oxm_match_field
        : public v13_detail::basic_oxm_type<OXMClass, OXMField>
    {
        using base_type = v13_detail::basic_oxm_type<OXMClass, OXMField>;

    public:
        using value_type = typename base_type::value_type;
        using needs_endian_conversion = typename base_type::needs_endian_conversion;

        explicit oxm_match_field(value_type value)
            : value_(std::move(value))
            , mask_{boost::none}
        {
        }

        oxm_match_field(value_type value, value_type mask)
            : value_(std::move(value))
            , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
        {
        }

        template <
              class T
            , typename std::enable_if<
                       !std::is_scalar<typename std::remove_reference<T>::type>::value
                    && !canard::is_related<oxm_match_field, T>::value
              >::type* = nullptr
        >
        explicit oxm_match_field(T&& value)
            : value_(std::forward<T>(value).to_bytes())
            , mask_{boost::none}
        {
        }

        template <class T, typename std::enable_if<!std::is_scalar<T>::value>::type* = nullptr>
        oxm_match_field(T&& value, value_type mask)
            : value_(std::forward<T>(value).to_bytes())
            , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
        {
        }

        auto oxm_header() const
            -> std::uint32_t
        {
            return (base_type::oxm_type() << 9) | (std::uint32_t{oxm_has_mask()} << 8) | oxm_length();
        }

        auto oxm_has_mask() const
            -> bool
        {
            return static_cast<bool>(mask_);
        }

        auto oxm_length() const
            -> std::uint8_t
        {
            return oxm_has_mask() ? sizeof(value_type) * 2 : sizeof(value_type);
        }

        auto oxm_value() const
            -> typename std::conditional<std::is_scalar<value_type>::value, value_type, value_type const&>::type
        {
            return value_;
        }

        auto oxm_mask() const
            -> boost::optional<value_type> const&
        {
            return mask_;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(oxm_header()) + oxm_length();
        }

        auto wildcard() const
            -> bool
        {
            return mask_ ? all_of(*mask_, 0x00) : false;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, oxm_header());
            detail::encode(container, value_, sizeof(value_), needs_endian_conversion{});
            return mask_ ? detail::encode(container, *mask_, sizeof(*mask_), needs_endian_conversion{}) : container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_match_field
        {
            auto const oxm_header = detail::decode<std::uint32_t>(first, last);
            auto const value = detail::decode<value_type>(first, last, sizeof(value_type), needs_endian_conversion{});
            if (oxm_header & 0x00000100) {
                auto const mask = detail::decode<value_type>(first, last, sizeof(value_type), needs_endian_conversion{});
                return oxm_match_field{value, mask};
            }
            return oxm_match_field{value};
        }

    private:
        static auto all_of(value_type const& mask, unsigned char value)
            -> bool
        {
            return boost::algorithm::all_of(canard::as_byte_range(mask)
                    , [=](unsigned char c) { return c == value; });
        }

    private:
        value_type value_;
        boost::optional<value_type> mask_;
    };

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT
    >::oxm_match_field(value_type value)
        : value_(value)
        , mask_{boost::none}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(port_no:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value)
        , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(port_no:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT
    >::oxm_match_field(value_type value)
        : value_(value)
        , mask_{boost::none}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw 3;
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value)
        , mask_{all_of(mask, 0xFF) ? boost::none : boost::optional<value_type>{mask}}
    {
        if (value_ == 0 || value_ > protocol::OFPP_MAX) {
            throw 3;
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_match_field(value_type value)
        : value_(value == protocol::OFPVID_NONE
                ? protocol::OFPVID_NONE : (value | protocol::OFPVID_PRESENT))
        , mask_(value == protocol::OFPVID_PRESENT
                ? boost::optional<value_type>{protocol::OFPVID_PRESENT} : boost::none)
    {
        if (value_ > 0x1FFF) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(vlan_vid:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_match_field(value_type value, value_type mask)
        : value_(value == protocol::OFPVID_NONE
                ? protocol::OFPVID_NONE : (value | protocol::OFPVID_PRESENT))
        , mask_((mask & 0xFFF) == 0xFFF
                ? boost::none : boost::optional<value_type>(mask | protocol::OFPVID_PRESENT))
    {
        if (value_ > 0x1FFF) {
            throw std::runtime_error{(boost::format{"%1%: oxm_value(vlan_vid:%2%) is invalid"} % __func__ % value_).str()};
        }
    }

    template <>
    inline auto oxm_match_field<
        protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID
    >::oxm_value() const
        -> value_type
    {
        return value_ == protocol::OFPVID_PRESENT
            ? protocol::OFPVID_PRESENT : value_ & ~protocol::OFPVID_PRESENT;
    }

    using oxm_in_port       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT>;
    using oxm_in_phy_port   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PHY_PORT>;
    using oxm_metadata      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_METADATA>;
    using oxm_eth_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST>;
    using oxm_eth_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC>;
    using oxm_eth_type      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE>;
    using oxm_vlan_vid      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID>;
    using oxm_vlan_pcp      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP>;
    using oxm_ip_dscp       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP>;
    using oxm_ip_ecn        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN>;
    using oxm_ip_proto      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO>;
    using oxm_ipv4_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC>;
    using oxm_ipv4_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST>;
    using oxm_tcp_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC>;
    using oxm_tcp_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST>;
    using oxm_udp_src       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC>;
    using oxm_udp_dst       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST>;
    using oxm_sctp_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC>;
    using oxm_sctp_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST>;
    using oxm_icmpv4_type   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_TYPE>;
    using oxm_icmpv4_code   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV4_CODE>;
    using oxm_arp_op        = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_OP>;
    using oxm_arp_spa       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SPA>;
    using oxm_arp_tpa       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_TPA>;
    using oxm_arp_sha       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_SHA>;
    using oxm_arp_tha       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ARP_THA>;
    using oxm_ipv6_src      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_SRC>;
    using oxm_ipv6_dst      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_DST>;
    using oxm_ipv6_flabel   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_FLABEL>;
    using oxm_icmpv6_type   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_TYPE>;
    using oxm_icmpv6_code   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ICMPV6_CODE>;
    using oxm_ipv6_nd_target= oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TARGET>;
    using oxm_ipv6_nd_sll   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_SLL>;
    using oxm_ipv6_nd_tll   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_ND_TLL>;
    using oxm_mpls_label    = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_LABEL>;
    using oxm_mpls_tc       = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_TC>;
    using oxm_mpls_bos      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_MPLS_BOS>;
    using oxm_pbb_isid      = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_PBB_ISID>;
    using oxm_tunnel_id     = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TUNNEL_ID>;
    using oxm_ipv6_exthdr   = oxm_match_field<protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV6_EXTHDR>;

} // namespace match

using oxm_in_port = match::oxm_in_port;
using oxm_in_phy_port = match::oxm_in_phy_port;
using oxm_metadata = match::oxm_metadata;
using oxm_eth_dst = match::oxm_eth_dst;
using oxm_eth_src = match::oxm_eth_src;
using oxm_eth_type = match::oxm_eth_type;
using oxm_vlan_vid = match::oxm_vlan_vid;
using oxm_vlan_pcp = match::oxm_vlan_pcp;
using oxm_ip_dscp = match::oxm_ip_dscp;
using oxm_ip_ecn = match::oxm_ip_ecn;
using oxm_ip_proto = match::oxm_ip_proto;
using oxm_ipv4_src = match::oxm_ipv4_src;
using oxm_ipv4_dst = match::oxm_ipv4_dst;
using oxm_tcp_src = match::oxm_tcp_src;
using oxm_tcp_dst = match::oxm_tcp_dst;
using oxm_udp_src = match::oxm_udp_src;
using oxm_udp_dst = match::oxm_udp_dst;
using oxm_sctp_src = match::oxm_sctp_src;
using oxm_sctp_dst = match::oxm_sctp_dst;
using oxm_icmpv4_type = match::oxm_icmpv4_type;
using oxm_icmpv4_code = match::oxm_icmpv4_code;
using oxm_arp_op = match::oxm_arp_op;
using oxm_arp_spa = match::oxm_arp_spa;
using oxm_arp_tpa = match::oxm_arp_tpa;
using oxm_arp_sha = match::oxm_arp_sha;
using oxm_arp_tha = match::oxm_arp_tha;
using oxm_ipv6_src = match::oxm_ipv6_src;
using oxm_ipv6_dst = match::oxm_ipv6_dst;
using oxm_ipv6_flabel = match::oxm_ipv6_flabel;
using oxm_icmpv6_type = match::oxm_icmpv6_type;
using oxm_icmpv6_code = match::oxm_icmpv6_code;
using oxm_ipv6_nd_target = match::oxm_ipv6_nd_target;
using oxm_ipv6_nd_sll = match::oxm_ipv6_nd_sll;
using oxm_ipv6_nd_tll = match::oxm_ipv6_nd_tll;
using oxm_mpls_label = match::oxm_mpls_label;
using oxm_mpls_tc = match::oxm_mpls_tc;
using oxm_mpls_bos = match::oxm_mpls_bos;
using oxm_pbb_isid = match::oxm_pbb_isid;
using oxm_tunnel_id = match::oxm_tunnel_id;
using oxm_ipv6_exthdr = match::oxm_ipv6_exthdr;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_HPP
