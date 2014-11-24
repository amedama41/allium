#ifndef CANARD_NETWORK_OPENFLOW_V10_ACTION_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ACTION_LIST_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/add_helper.hpp>
#include <canard/network/protocol/openflow/v10/any_action.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class action_list
    {
    public:
        template <
              class... Actions
            , typename std::enable_if<!canard::is_related<action_list, Actions...>::value>::type* = nullptr
        >
        action_list(Actions&&... actions)
            : actions_{any_action(std::forward<Actions>(actions))...}
        {
        }

        auto length() const
            -> std::uint16_t
        {
            using boost::adaptors::transformed;
            return boost::accumulate(actions_ | transformed([](any_action const& action) {
                        return action.length();
            }), std::uint16_t{0});
        }

        template <class Action>
        void add(Action&& action)
        {
            actions_.emplace_back(std::forward<Action>(action));
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            boost::for_each(actions_, [&](any_action const& action) {
                action.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_list
        {
            auto actions = action_list{};
            while (first != last) {
                detail::decode_action<void>(first, last
                        , openflow::detail::add_helper<action_list>{actions});
            }
            return actions;
        }

    private:
        std::vector<any_action> actions_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ACTION_LIST_HPP
