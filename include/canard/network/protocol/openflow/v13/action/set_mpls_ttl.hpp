#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_MPLS_TTL_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_MPLS_TTL_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class set_mpls_ttl
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_SET_MPLS_TTL;

            explicit set_mpls_ttl(std::uint8_t const mpls_ttl)
                : mpls_ttl_{action_type, sizeof(v13_detail::ofp_action_mpls_ttl), mpls_ttl}
            {
            }

            auto type() const
                -> protocol::ofp_action_type
            {
                return action_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_action_mpls_ttl);
            }

            auto ttl() const
                -> std::uint8_t
            {
                return mpls_ttl_.mpls_ttl;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, mpls_ttl_);
            }

        private:
            explicit set_mpls_ttl(v13_detail::ofp_action_mpls_ttl const& action_mpls_ttl)
                : mpls_ttl_(action_mpls_ttl)
            {
                if (mpls_ttl_.type != type()) {
                    throw 1;
                }
                if (mpls_ttl_.len != length()) {
                    throw 2;
                }
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> set_mpls_ttl
            {
                auto const action_mpls_ttl = detail::decode<v13_detail::ofp_action_mpls_ttl>(first, last);
                return set_mpls_ttl{action_mpls_ttl};
            }

        private:
            v13_detail::ofp_action_mpls_ttl mpls_ttl_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_MPLS_TTL_HPP
