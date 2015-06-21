#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_NW_TTL_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_NW_TTL_HPP

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

        class set_nw_ttl
        {
        public:
            static protocol::ofp_action_type const action_type
                = protocol::OFPAT_SET_NW_TTL;

            explicit set_nw_ttl(std::uint8_t const nw_ttl)
                : nw_ttl_{action_type, sizeof(v13_detail::ofp_action_nw_ttl), nw_ttl, {0}}
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
                return sizeof(v13_detail::ofp_action_nw_ttl);
            }

            auto ttl() const
                -> std::uint8_t
            {
                return nw_ttl_.nw_ttl;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, nw_ttl_);
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> set_nw_ttl
            {
                auto const action_nw_ttl = detail::decode<v13_detail::ofp_action_nw_ttl>(first, last);
                if (action_nw_ttl.type != action_type) {
                    throw 1;
                }
                if (action_nw_ttl.len != sizeof(v13_detail::ofp_action_nw_ttl)) {
                    throw 2;
                }
                return set_nw_ttl{action_nw_ttl.nw_ttl};
            }

        private:
            v13_detail::ofp_action_nw_ttl nw_ttl_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_NW_TTL_HPP
