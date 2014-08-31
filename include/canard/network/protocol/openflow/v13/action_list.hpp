#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/network/protocol/openflow/v13/any_action.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_list
    {
    public:
        action_list()
            : actions_{}
        {
        }

        template <class... Actions, typename std::enable_if<!is_related<action_list, Actions...>::value>::type* = nullptr>
        action_list(Actions&&... actions)
            : actions_{any_action{std::forward<Actions>(actions)}...}
        {
        }

        action_list(action_list const&) = default;
        action_list(action_list&&) noexcept = default;
        action_list& operator=(action_list&&) noexcept = default;

        action_list& operator=(action_list const& other)
        {
            auto tmp = action_list{other};
            swap(tmp);
            return *this;
        }

        void swap(action_list& other)
        {
            this->actions_.swap(other.actions_);
        }

        auto length() const
            -> std::uint16_t
        {
            using boost::adaptors::transformed;
            return boost::accumulate(
                      actions_ | transformed([](any_action const& action){ return action.length(); })
                    , std::uint16_t{0});
        }

        template <class Action>
        void add(Action&& action)
        {
            actions_.push_back(std::forward<Action>(action));
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
            auto act_list = action_list{};
            while (first != last) {
                act_list.add(decode_action(first, last));
            }
            return act_list;
        }

    private:
        std::vector<any_action> actions_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_LIST_HPP
