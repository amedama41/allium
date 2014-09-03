#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP

#include <cstdint>
#include <boost/format.hpp>
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
        action_id(std::uint16_t const type)
            : type_{type}
        {
        }

        auto type() const
            -> ofp_action_type
        {
            return ofp_action_type(type_);
        }

        static constexpr auto length()
            -> std::uint16_t
        {
            return sizeof(std::uint32_t);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(detail::encode(container, type_), length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_id
        {
            auto const type = detail::decode<std::uint16_t>(first, last);
            auto const length = detail::decode<std::uint16_t>(first, last);
            if (length != sizeof(std::uint16_t) + sizeof(std::uint16_t)) {
                throw std::runtime_error{
                    (boost::format{"%s: ofp_action_header:length is invalid, expected 4 but %u"} % __func__ % length).str()
                };
            }
            return action_id{type};
        }

    private:
        std::uint16_t type_;
    };

    template <class Iterator>
    inline auto decode_action_id(Iterator& first, Iterator last)
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
