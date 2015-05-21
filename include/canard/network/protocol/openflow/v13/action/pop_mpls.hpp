#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_MPLS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_MPLS_HPP

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

        class pop_mpls
        {
        public:
            static ofp_action_type const action_type = OFPAT_POP_MPLS;

            explicit pop_mpls(std::uint16_t const ethertype)
                : pop_mpls_{action_type, sizeof(v13_detail::ofp_action_pop_mpls), ethertype, {0}}
            {
            }

            auto type() const
                -> ofp_action_type
            {
                return action_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(v13_detail::ofp_action_pop_mpls);
            }

            auto ethertype() const
                -> std::uint16_t
            {
                return pop_mpls_.ethertype;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, pop_mpls_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> pop_mpls
            {
                auto const action_pop_mpls = detail::decode<v13_detail::ofp_action_pop_mpls>(first, last);
                if (action_pop_mpls.type != action_type) {
                    throw 1;
                }
                if (action_pop_mpls.len != sizeof(v13_detail::ofp_action_pop_mpls)) {
                    throw 2;
                }
                return pop_mpls{action_pop_mpls.ethertype};
            }

        private:
            v13_detail::ofp_action_pop_mpls pop_mpls_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_POP_MPLS_HPP
