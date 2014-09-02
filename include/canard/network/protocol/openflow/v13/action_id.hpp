#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/v13/any_action_id.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_id
    {
    public:
        action_id(std::uint16_t const type, std::uint16_t const length)
            : type_{type}
            , length_{length}
        {
        }

        auto type() const
            -> ofp_action_type
        {
            return ofp_action_type(type_);
        }

        static constexpr auto length() const
            -> std::uint16_t
        {
            return sizeof(std::uint32_t);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(detail::encode(container, type_), length_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_id
        {
            auto const type = detail::decode<std::uint16_t>(first, last);
            auto const length = detail::decode<std::uint16_t>(first, last);
            return action_id{type, length};
        }

    private:
        std::uint16_t type_;
        std::uint16_t length_;
    };

    template <class Iterator>
    inline auto decode(Iterator& first, Iterator last)
        -> any_action_id
    {
        auto copy_first = first;
        auto const type = detail::decode<std::uint16_t>(copy_first, last);
        switch (type) {
        default:
            return action_id::decode(first, last);
        }
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
