#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP

#include <cstdint>
#include <map>
#include <type_traits>
#include <utility>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/any_action.hpp>
#include <canard/network/protocol/openflow/v13/detail/add_helper.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode_action.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_set
    {
    public:
        action_set()
            : action_map_{}
        {
        }

        template <class... Actions, typename std::enable_if<!is_related<action_set, Actions...>::value>::type* = nullptr>
        action_set(Actions&&... actions)
            : action_map_{}
        {
            add_impl(std::forward<Actions>(actions)...);
        }

        template <class Action>
        void add(Action&& action)
        {
            auto const order = action_order(action);
            auto const it = action_map_.lower_bound(order);
            if (it != action_map_.end() && !action_map_.key_comp()(order, it->first)) {
                it->second = std::forward<Action>(action);
            }
            else {
                action_map_.emplace_hint(it, order, std::forward<Action>(action));
            }
        }

        auto length() const
            -> std::uint16_t
        {
            using boost::adaptors::map_values;
            using boost::adaptors::transformed;
            return boost::accumulate(action_map_ | map_values | transformed([](any_action const& action) {
                    return action.length();
            }), std::uint16_t{0});
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            using boost::adaptors::map_values;
            boost::for_each(action_map_ | map_values, [&](any_action const& action) {
                action.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_set
        {
            auto act_set = action_set{};
            while (first != last) {
                detail::decode_action<void>(first, last, detail::add_helper<action_set>{act_set});
            }
            return act_set;
        }

    private:
        void add_impl()
        {
        }

        template <class Action, class... Actions>
        void add_impl(Action&& action, Actions&&... actions)
        {
            add(std::forward<Action>(action));
            add_impl(std::forward<Actions>(actions)...);
        }

    private:
        std::map<std::uint64_t, any_action> action_map_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP
