#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_FIELD_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace actions {

    template <class OXMMatchField>
    class set_field
        : private boost::equality_comparable<set_field<OXMMatchField>>
    {
    public:
        static constexpr protocol::ofp_action_type action_type
            = protocol::OFPAT_SET_FIELD;
        static constexpr std::size_t base_size
            = offsetof(v13_detail::ofp_action_set_field, field);

        using value_type = typename OXMMatchField::value_type;

        explicit set_field(value_type const& value)
            : field_{value}
        {
        }

        static constexpr auto type() noexcept
            -> protocol::ofp_action_type
        {
            return action_type;
        }

        static constexpr auto oxm_type() noexcept
            -> std::uint32_t
        {
            return OXMMatchField::oxm_type();
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return detail::v13::exact_length(base_size + field_.length());
        }

        auto value() const noexcept
            -> value_type
        {
            return field_.oxm_value();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t{type()});
            detail::encode(container, length());
            field_.encode(container);
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , detail::v13::padding_length(base_size + field_.length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> set_field
        {
            std::advance(first, sizeof(v13_detail::ofp_action_set_field::type));
            auto const length = detail::decode<std::uint16_t>(first, last);
            auto field = OXMMatchField::decode(first, last);
            std::advance(first, length - (base_size + field.length()));
            return set_field{std::move(field)};
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> set_field
        {
            return validate(set_field(std::forward<Args>(args)...));
        }

        static void validate_set_field(
                v13_detail::ofp_action_set_field const& set_field)
        {
            auto it = set_field.field;
            auto const oxm_header = detail::decode<std::uint32_t>(
                    it, it + sizeof(set_field.field));

            OXMMatchField::validate_oxm_header(oxm_header);
            if (oxm_header & 0x00000100) {
                throw std::runtime_error{"invalid oxm_hasmask"};
            }
            if (set_field.len != detail::v13::exact_length(
                        base_size + (oxm_header & 0xff))) {
                throw std::runtime_error{"invalid set_field length"};
            }
        }

        template <class Action>
        static auto validate(Action&& action)
            -> typename std::enable_if<
                  canard::is_same_value_type<Action, set_field>::value
                , Action&&
               >::type
        {
            OXMMatchField::validate(action.field_);
            if (action.field_.oxm_has_mask()) {
                throw std::runtime_error{"invalid oxm_hasmask"};
            }
            return std::forward<Action>(action);
        }

        friend auto operator==(
                set_field const& lhs, set_field const& rhs) noexcept
            -> bool
        {
            return lhs.field_ == rhs.field_;
        }

    private:
        set_field(OXMMatchField&& field)
            : field_(std::move(field))
        {
        }

    private:
        OXMMatchField field_;
    };

    using set_eth_dst        = set_field<oxm_match::eth_dst>;
    using set_eth_src        = set_field<oxm_match::eth_src>;
    using set_eth_type       = set_field<oxm_match::eth_type>;
    using set_vlan_vid       = set_field<oxm_match::vlan_vid>;
    using set_vlan_pcp       = set_field<oxm_match::vlan_pcp>;
    using set_ip_dscp        = set_field<oxm_match::ip_dscp>;
    using set_ip_ecn         = set_field<oxm_match::ip_ecn>;
    using set_ip_proto       = set_field<oxm_match::ip_proto>;
    using set_ipv4_src       = set_field<oxm_match::ipv4_src>;
    using set_ipv4_dst       = set_field<oxm_match::ipv4_dst>;
    using set_tcp_src        = set_field<oxm_match::tcp_src>;
    using set_tcp_dst        = set_field<oxm_match::tcp_dst>;
    using set_udp_src        = set_field<oxm_match::udp_src>;
    using set_udp_dst        = set_field<oxm_match::udp_dst>;
    using set_sctp_src       = set_field<oxm_match::sctp_src>;
    using set_sctp_dst       = set_field<oxm_match::sctp_dst>;
    using set_icmpv4_type    = set_field<oxm_match::icmpv4_type>;
    using set_icmpv4_code    = set_field<oxm_match::icmpv4_code>;
    using set_arp_op         = set_field<oxm_match::arp_op>;
    using set_arp_spa        = set_field<oxm_match::arp_spa>;
    using set_arp_tpa        = set_field<oxm_match::arp_tpa>;
    using set_arp_sha        = set_field<oxm_match::arp_sha>;
    using set_arp_tha        = set_field<oxm_match::arp_tha>;
    using set_ipv6_src       = set_field<oxm_match::ipv6_src>;
    using set_ipv6_dst       = set_field<oxm_match::ipv6_dst>;
    using set_ipv6_flabel    = set_field<oxm_match::ipv6_flabel>;
    using set_icmpv6_type    = set_field<oxm_match::icmpv6_type>;
    using set_icmpv6_code    = set_field<oxm_match::icmpv6_code>;
    using set_ipv6_nd_target = set_field<oxm_match::ipv6_nd_target>;
    using set_ipv6_nd_sll    = set_field<oxm_match::ipv6_nd_sll>;
    using set_ipv6_nd_tll    = set_field<oxm_match::ipv6_nd_tll>;
    using set_mpls_label     = set_field<oxm_match::mpls_label>;
    using set_mpls_tc        = set_field<oxm_match::mpls_tc>;
    using set_mpls_bos       = set_field<oxm_match::mpls_bos>;
    using set_pbb_isid       = set_field<oxm_match::pbb_isid>;
    using set_tunnel_id      = set_field<oxm_match::tunnel_id>;

} // namespace actions

using default_set_field_list = std::tuple<
      actions::set_eth_dst
    , actions::set_eth_src
    , actions::set_eth_type
    , actions::set_vlan_vid
    , actions::set_vlan_pcp
    , actions::set_ip_dscp
    , actions::set_ip_ecn
    , actions::set_ip_proto
    , actions::set_ipv4_src
    , actions::set_ipv4_dst
    , actions::set_tcp_src
    , actions::set_tcp_dst
    , actions::set_udp_src
    , actions::set_udp_dst
    , actions::set_sctp_src
    , actions::set_sctp_dst
    , actions::set_icmpv4_type
    , actions::set_icmpv4_code
    , actions::set_arp_op
    , actions::set_arp_spa
    , actions::set_arp_tpa
    , actions::set_arp_sha
    , actions::set_arp_tha
    , actions::set_ipv6_src
    , actions::set_ipv6_dst
    , actions::set_ipv6_flabel
    , actions::set_icmpv6_type
    , actions::set_icmpv6_code
    , actions::set_ipv6_nd_target
    , actions::set_ipv6_nd_sll
    , actions::set_ipv6_nd_tll
    , actions::set_mpls_label
    , actions::set_mpls_tc
    , actions::set_mpls_bos
    , actions::set_pbb_isid
    , actions::set_tunnel_id
>;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_SET_FIELD_HPP
