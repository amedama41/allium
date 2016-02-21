#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTIONS_BASIC_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTIONS_BASIC_ACTION_HPP

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace actions_detail {

    template <class T, class OFPAction>
    class basic_action
    {
    protected:
        basic_action() = default;

    public:
        auto type() const noexcept
            -> protocol::ofp_action_type
        {
            return T::action_type;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return sizeof(OFPAction);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, base_action());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            return T{detail::decode<OFPAction>(first, last)};
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> T
        {
            return T::validate(T{std::forward<Args>(args)...});
        }

        static void validate_header(v10_detail::ofp_action_header const& header)
        {
            if (header.type != T::action_type) {
                throw std::runtime_error{"invalid action type"};
            }
            if (header.len != sizeof(OFPAction)) {
                throw std::runtime_error{"invalid action length"};
            }
        }

    private:
        auto base_action() const noexcept
            -> OFPAction const&
        {
            return static_cast<T const*>(this)->ofp_action();
        }
    };

} // namespace actions_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTIONS_BASIC_ACTION_HPP
