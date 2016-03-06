#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ACTION_PUSH_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ACTION_PUSH_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        namespace action_detail {

            template <class T>
            class action_push
            {
            protected:
                explicit action_push(std::uint16_t const ethertype)
                    : push_{T::action_type, sizeof(v13_detail::ofp_action_push), ethertype, {0}}
                {
                }

            public:
                auto type() const
                    -> protocol::ofp_action_type
                {
                    return T::action_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    return sizeof(v13_detail::ofp_action_push);
                }

                auto ethertype() const
                    -> std::uint16_t
                {
                    return push_.ethertype;
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    return detail::encode(container, push_);
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const action_push = detail::decode<v13_detail::ofp_action_push>(first, last);
                    if (action_push.type != T::action_type) {
                        throw 1;
                    }
                    if (action_push.len != sizeof(v13_detail::ofp_action_push)) {
                        throw 2;
                    }
                    return T{action_push.ethertype};
                }

            private:
                v13_detail::ofp_action_push push_;
            };

        } // namespace action_detail

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_ACTION_PUSH_HPP
