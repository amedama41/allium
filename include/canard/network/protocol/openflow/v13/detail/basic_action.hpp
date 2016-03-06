#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

    template <class T, class OFPAction>
    class basic_action
        : private boost::equality_comparable<T>
    {
    protected:
        using raw_ofp_type = OFPAction;

        basic_action() = default;

    public:
        static constexpr auto type() noexcept
            -> openflow::v13::protocol::ofp_action_type
        {
            return T::action_type;
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(raw_ofp_type);
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
            return T{detail::decode<raw_ofp_type>(first, last)};
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> T
        {
            return T::validate(T(std::forward<Args>(args)...));
        }

        static void validate_action_header(
                openflow::v13::v13_detail::ofp_action_header const& header)
        {
            if (header.type != T::action_type) {
                throw std::runtime_error{"invalid action type"};
            }
            if (header.len != sizeof(raw_ofp_type)) {
                throw std::runtime_error{"invalid action length"};
            }
        }

        template <class Action>
        static auto validate(Action&& action)
            -> typename std::enable_if<
                  canard::is_same_value_type<Action, T>::value
                , Action&&
               >::type
        {
            T::validate_impl(action);
            return std::forward<Action>(action);
        }

    private:
        auto base_action() const noexcept
            -> raw_ofp_type const&
        {
            return static_cast<T const*>(this)->ofp_action();
        }
    };

} // namespace v13
} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_V13_BASIC_ACTION_HPP
