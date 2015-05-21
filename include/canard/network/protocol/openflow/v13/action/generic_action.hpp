#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_GENERIC_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_GENERIC_ACTION_HPP

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

        namespace action_detail {

            template <class T>
            class generic_action
            {
            protected:
                generic_action()
                    : header_{T::action_type, sizeof(v13_detail::ofp_action_header), {0}}
                {
                }

            public:
                auto type() const
                    -> ofp_action_type
                {
                    return T::action_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    return sizeof(v13_detail::ofp_action_header);
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    return detail::encode(container, header_);
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const header = detail::decode<v13_detail::ofp_action_header>(first, last);
                    if (header.type != T::action_type) {
                        throw 1;
                    }
                    if (header.len != sizeof(v13_detail::ofp_action_header)) {
                        throw 2;
                    }
                    return T{};
                }

            private:
                v13_detail::ofp_action_header header_;
            };

        } // namespace action_detail

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_GENERIC_ACTION_HPP
