#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <boost/optional/optional.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/lower_bound.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <canard/network/protocol/openflow/v13/actions.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_set
        : private boost::equality_comparable<action_set>
    {
    public:
        using key_type = std::uint64_t;
        using value_type = action_list::value_type;
        using reference = action_list::const_reference;
        using const_reference = action_list::const_reference;
        using iterator = action_list::const_iterator;
        using const_iterator = action_list::const_iterator;
        using difference_type = action_list::difference_type;
        using size_type = action_list::size_type;
        using reverse_iterator = action_list::reverse_iterator;
        using const_reverse_iterator = action_list::const_reverse_iterator;

        action_set() = default;

        template <
              class... Actions
            , typename std::enable_if<
                !canard::is_related<action_set, Actions...>::value
              >::type* = nullptr
        >
        action_set(Actions&&... actions)
        {
            std::pair<const_iterator, bool> dummy[] = {
                insert(std::forward<Actions>(actions))...
            };
            static_cast<void>(dummy);
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return actions_.begin();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return actions_.end();
        }

        auto cbegin() const noexcept
            -> const_iterator
        {
            return actions_.cbegin();
        }

        auto cend() const noexcept
            -> const_iterator
        {
            return actions_.cend();
        }

        auto rbegin() const noexcept
            -> const_reverse_iterator
        {
            return actions_.rbegin();
        }

        auto rend() const noexcept
            -> const_reverse_iterator
        {
            return actions_.rend();
        }

        auto crbegin() const noexcept
            -> const_reverse_iterator
        {
            return actions_.crbegin();
        }

        auto crend() const noexcept
            -> const_reverse_iterator
        {
            return actions_.crend();
        }

        auto empty() const noexcept
            -> bool
        {
            return actions_.empty();
        }

        auto size() const noexcept
            -> size_type
        {
            return actions_.size();
        }

        auto max_size() const noexcept
            -> size_type
        {
            return actions_.max_size();
        }

        auto at(key_type const act_order) const
            -> const_reference
        {
            auto const it = find(act_order);
            if (it == actions_.end()) {
                throw std::out_of_range{"not found specified action"};
            }
            return *it;
        }

        template <class Action>
        auto at() const
            -> Action const&
        {
            return v13::any_cast<Action>(at(action_order<Action>::value));
        }

        template <class Action>
        auto get() const
            -> Action const&
        {
            auto const it = find(action_order<Action>::value);
            return v13::any_cast<Action>(*it);
        }

        template <class Action>
        auto insert(Action&& action)
            -> std::pair<const_iterator, bool>
        {
            auto const order = get_order(action);
            auto const pos = non_const_lower_bound(order);
            if (pos != actions_.end() && get_order(*pos) == order) {
                return std::make_pair(const_iterator{pos}, false);
            }
            else {
                auto const it = actions_.insert(
                        pos, value_type{std::forward<Action>(action)});
                return std::make_pair(const_iterator{it}, true);
            }
        }

        template <class Action>
        auto assign(Action&& action)
            -> std::pair<const_iterator, bool>
        {
            auto const order = get_order(action);
            auto const pos = non_const_lower_bound(order);
            if (pos != actions_.end() && get_order(*pos) == order) {
                *pos = std::forward<Action>(action);
                return std::make_pair(const_iterator{pos}, true);
            }
            else {
                return std::make_pair(const_iterator{actions_.end()}, false);
            }
        }

        template <class Action>
        auto insert_or_assign(Action&& action)
            -> std::pair<const_iterator, bool>
        {
            auto const order = get_order(action);
            auto const pos = non_const_lower_bound(order);
            if (pos != actions_.end() && get_order(*pos) == order) {
                *pos = std::forward<Action>(action);
                return std::make_pair(const_iterator{pos}, false);
            }
            else {
                auto const it = actions_.insert(
                        pos, value_type{std::forward<Action>(action)});
                return std::make_pair(const_iterator{it}, true);
            }
        }

        auto erase(const_iterator it)
            -> const_iterator
        {
            return actions_.erase(it);
        }

        template <class Action>
        auto erase()
            -> size_type
        {
            auto const it = find(action_order<Action>::value);
            if (it == actions_.end()) {
                return 0;
            }
            actions_.erase(it);
            return 1;
        }

        void swap(action_set& other) noexcept
        {
            actions_.swap(other.actions_);
        }

        void clear() noexcept
        {
            return actions_.clear();
        }

        auto find(key_type const act_order) const
            -> const_iterator
        {
            return boost::find_if(actions_, [=](const_reference e) {
                    return get_order(e) == act_order;
            });
        }

        template <class Action>
        auto find() const
            -> boost::optional<Action const&>
        {
            auto const it = find(action_order<Action>::value);
            if (it == actions_.end()) {
                return boost::none;
            }
            return v13::any_cast<Action>(*it);
        }

        auto length() const
            -> std::uint16_t
        {
            return actions_.length();
        }

        auto to_list() const&
            -> action_list const&
        {
            return actions_;
        }

        auto to_list() &&
            -> action_list
        {
            return actions_;
        }

    private:
        auto non_const_lower_bound(key_type const act_order)
            -> action_list::iterator
        {
            return boost::lower_bound(
                      actions_
                    , act_order
                    , [](const_reference e, key_type const act_order)
                      { return get_order(e) < act_order; });
        }

    private:
        action_list actions_;
    };

    auto operator==(action_set const& lhs, action_set const& rhs)
        -> bool
    {
        return lhs.to_list() == rhs.to_list();
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_HPP
