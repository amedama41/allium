#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_ACTION_LIST_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_ACTION_LIST_HPP

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/operators.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    template <class T>
    auto is_input_iterator_impl(T&)
        -> std::is_base_of<
              std::input_iterator_tag
            , typename std::iterator_traits<T>::iterator_category
           >;

    auto is_input_iterator_impl(...)
        -> std::false_type;

    template <class... T>
    struct is_input_iterator : std::false_type
    {};

    template <class T>
    struct is_input_iterator<T>
        : decltype(is_input_iterator_impl(std::declval<T>()))
    {};


    template <class AnyAction, class ActionHeader>
    class action_list
        : private boost::equality_comparable<
            action_list<AnyAction, ActionHeader>
          >
    {
        using container = std::vector<AnyAction>;

    public:
        using value_type = typename container::value_type;
        using reference = typename container::reference;
        using const_reference = typename container::const_reference;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
        using difference_type = typename container::difference_type;
        using size_type = typename container::size_type;

        using reverse_iterator = typename container::reverse_iterator;
        using const_reverse_iterator
            = typename container::const_reverse_iterator;

        template <
              class... Actions
            , typename std::enable_if<
                    !canard::is_related<action_list, Actions...>::value
              >::type* = nullptr
        >
        action_list(Actions&&... actions)
            : actions_{value_type(std::forward<Actions>(actions))...}
        {
        }

        auto begin() noexcept
            -> iterator
        {
            return actions_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return actions_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return actions_.end();
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

        auto rbegin() noexcept
            -> reverse_iterator
        {
            return actions_.rbegin();
        }

        auto rbegin() const noexcept
            -> const_reverse_iterator
        {
            return actions_.rbegin();
        }

        auto rend() noexcept
            -> reverse_iterator
        {
            return actions_.rend();
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

        void swap(action_list& other) noexcept
        {
            actions_.swap(other.actions_);
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

        auto empty() const noexcept
            -> bool
        {
            return actions_.empty();
        }

        void clear() noexcept
        {
            return actions_.clear();
        }

        auto operator[](size_type const n)
            -> reference
        {
            return actions_[n];
        }

        auto operator[](size_type const n) const
            -> const_reference
        {
            return actions_[n];
        }

        auto at(size_type const n)
            -> reference
        {
            return actions_.at(n);
        }

        auto at(size_type const n) const
            -> const_reference
        {
            return actions_.at(n);
        }

        template <class Action>
        void push_back(Action&& action)
        {
            actions_.emplace_back(std::forward<Action>(action));
        }

        void pop_back()
        {
            actions_.pop_back();
        }

        template <class... Actions>
        auto insert(const_iterator pos, Actions&&... actions)
            -> typename std::enable_if<
                  !is_input_iterator<Actions...>::value, iterator
               >::type
        {
            return actions_.insert(
                    pos, { value_type(std::forward<Actions>(actions))... });
        }

        template <class InputIterator>
        auto insert(const_iterator pos, InputIterator first, InputIterator last)
            -> typename std::enable_if<
                  is_input_iterator<InputIterator>::value, iterator
               >::type
        {
            return actions_.insert(pos, first, last);
        }

        auto erase(const_iterator pos)
            -> iterator
        {
            return actions_.erase(pos);
        }

        auto erase(const_iterator first, const_iterator last)
            -> iterator
        {
            return actions_.erase(first, last);
        }

        auto length() const
            -> std::size_t
        {
            return boost::accumulate(
                      actions_ | boost::adaptors::transformed(
                          [](value_type const& action) { return action.length(); })
                    , std::size_t{0});
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            boost::for_each(actions_, [&](value_type const& action) {
                action.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_list
        {
            auto actions = std::vector<value_type>{};
            actions.reserve(std::distance(first, last) / sizeof(ActionHeader));
            while (std::distance(first, last) >= sizeof(ActionHeader)) {
                actions.push_back(value_type::decode(first, last));
            }
            if (first != last) {
                throw std::runtime_error{"action_list length is invalid"};
            }
            return action_list{std::move(actions)};
        }

        friend auto operator==(action_list const& lhs, action_list const& rhs)
            -> bool
        {
            return lhs.actions_ == rhs.actions_;
        }

    private:
        explicit action_list(std::vector<value_type>&& actions)
            : actions_(std::move(actions))
        {
        }

    private:
        std::vector<value_type> actions_;
    };

    template <class AnyAction, class ActionHeader>
    void swap(action_list<AnyAction, ActionHeader>& lhs
            , action_list<AnyAction, ActionHeader>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_ACTION_LIST_HPP
