#ifndef CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP

#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/optional/optional.hpp>
#include <canard/network/protocol/openflow/v10/match_fields.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class match_set
        : boost::equality_comparable<match_set>
    {
        using raw_ofp_type = v10_detail::ofp_match;

    public:
        template <
              class... MatchFields
            , typename std::enable_if<
                       !canard::is_related<match_set, MatchFields...>::value
                    && !canard::is_related<raw_ofp_type, MatchFields...>::value
              >::type* = nullptr
        >
        explicit match_set(MatchFields&&... fields) noexcept
            : match_{protocol::OFPFW_ALL, 0}
        {
            set_impl(std::forward<MatchFields>(fields)...);
        }

        explicit match_set(raw_ofp_type const& match) noexcept
            : match_(match)
        {
        }

        auto empty() const noexcept
            -> bool
        {
            constexpr std::uint32_t nw_addr_mask
                = protocol::OFPFW_NW_SRC_ALL | protocol::OFPFW_NW_DST_ALL;
            constexpr std::uint32_t all_mask_wo_nw_addr
                = protocol::OFPFW_ALL
                & ~(protocol::OFPFW_NW_SRC_MASK | protocol::OFPFW_NW_DST_MASK);

            return ((wildcards() & nw_addr_mask) == nw_addr_mask)
                && ((wildcards() & all_mask_wo_nw_addr) == all_mask_wo_nw_addr);
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return sizeof(raw_ofp_type);
        }

        auto wildcards() const noexcept
            -> std::uint32_t
        {
            return match_.wildcards;
        }

        template <class MatchField>
        auto get() const noexcept
            -> boost::optional<MatchField>
        {
            if (MatchField::is_wildcard(ofp_match())) {
                return boost::none;
            }
            return MatchField::create_from_match(ofp_match());
        }

        template <class MatchField>
        void add(MatchField&& field) noexcept
        {
            set(std::forward<MatchField>(field));
        }

        template <class MatchField>
        void set(MatchField&& field) noexcept
        {
            std::forward<MatchField>(field).set_value(match_);
        }

        template <class MatchField>
        void erase() noexcept
        {
            MatchField::erase_from_match(match_);
        }

        auto ofp_match() const noexcept
            -> raw_ofp_type const&
        {
            return match_;
        }

    private:
        void set_impl() noexcept
        {
        }

        template <class MatchField, class... MatchFields>
        void set_impl(MatchField&& field, MatchFields&&... fields) noexcept
        {
            set(std::forward<MatchField>(field));
            set_impl(std::forward<MatchFields>(fields)...);
        }

    private:
        raw_ofp_type match_;
    };

    auto operator==(match_set const& lhs, match_set const& rhs) noexcept
        -> bool
    {
        return lhs.get<match::in_port>() == rhs.get<match::in_port>()
            && lhs.get<match::eth_src>() == rhs.get<match::eth_src>()
            && lhs.get<match::eth_dst>() == rhs.get<match::eth_dst>()
            && lhs.get<match::vlan_vid>() == rhs.get<match::vlan_vid>()
            && lhs.get<match::vlan_pcp>() == rhs.get<match::vlan_pcp>()
            && lhs.get<match::eth_type>() == rhs.get<match::eth_type>()
            && lhs.get<match::ip_dscp>() == rhs.get<match::ip_dscp>()
            && lhs.get<match::ip_proto>() == rhs.get<match::ip_proto>()
            && lhs.get<match::ipv4_src>() == rhs.get<match::ipv4_src>()
            && lhs.get<match::ipv4_dst>() == rhs.get<match::ipv4_dst>()
            && lhs.get<match::tcp_src>() == rhs.get<match::tcp_src>()
            && lhs.get<match::tcp_dst>() == rhs.get<match::tcp_dst>()
            ;
    }

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP
