#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTION_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTION_ADAPTOR_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace v10_detail {

    template <class T, class OFPAction>
    class action_adaptor
    {
    protected:
        action_adaptor() = default;

    public:
        auto type() const
            -> ofp_action_type
        {
            return T::action_type;
        }

        auto length() const
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
            auto const ofp_action = detail::decode<OFPAction>(first, last);
            if (ofp_action.type != T::action_type) {
                throw std::runtime_error{"invalid action type"};
            }
            if (ofp_action.len != sizeof(OFPAction)) {
                throw std::runtime_error{"invalid action length"};
            }
            return T{ofp_action};
        }

    private:
        auto base_action() const
            -> OFPAction const&
        {
            return static_cast<T const*>(this)->ofp_action();
        }
    };

} // namespace v10_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTION_ADAPTOR_HPP
