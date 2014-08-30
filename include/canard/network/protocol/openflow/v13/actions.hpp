#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>
#include <boost/format.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/any_action.hpp>
#include <canard/network/protocol/openflow/v13/action/output.hpp>
#include <canard/network/protocol/openflow/v13/action/copy_ttl_out.hpp>
#include <canard/network/protocol/openflow/v13/action/copy_ttl_in.hpp>
#include <canard/network/protocol/openflow/v13/action/set_mpls_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/decrement_mpls_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/push_vlan.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_vlan.hpp>
#include <canard/network/protocol/openflow/v13/action/push_mpls.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_mpls.hpp>
#include <canard/network/protocol/openflow/v13/action/set_queue.hpp>
#include <canard/network/protocol/openflow/v13/action/group.hpp>
#include <canard/network/protocol/openflow/v13/action/set_nw_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/decrement_nw_ttl.hpp>
#include <canard/network/protocol/openflow/v13/action/set_field.hpp>
#include <canard/network/protocol/openflow/v13/action/push_pbb.hpp>
#include <canard/network/protocol/openflow/v13/action/pop_pbb.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    template <class ReturnType, class Iterator, class Function>
    auto decode_action(Iterator& first, Iterator last, Function function)
        -> ReturnType
    {
        auto header = detail::ofp_action_header{};
        std::copy_n(first, sizeof(header), canard::as_byte_range(header).begin());
        using action_list = std::tuple<
              actions::output
            , actions::copy_ttl_out
            , actions::copy_ttl_in
            , actions::set_mpls_ttl
            , actions::decrement_mpls_ttl
            , actions::push_vlan
            , actions::pop_vlan
            , actions::push_mpls
            , actions::pop_mpls
            , actions::set_queue
            , actions::group
            , actions::set_nw_ttl
            , actions::decrement_nw_ttl
            , actions::set_field
            , actions::push_pbb
            , actions::pop_pbb
        >;
        static_assert(std::tuple_size<action_list>::value == 16, "");
        switch (detail::ntoh(header.type)) {
#       define CANARD_NETWORK_OPENFLOW_DECODE_ACTION_CASE(z, N, _) \
        case std::tuple_element<N, action_list>::type::action_type: \
            return function(std::tuple_element<N, action_list>::type::decode(first, last));
        BOOST_PP_REPEAT(16, CANARD_NETWORK_OPENFLOW_DECODE_ACTION_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_DECODE_ACTION_CASE

        default:
            std::advance(first, detail::ntoh(header.len));
            throw std::runtime_error{(boost::format{"%1%: action_type(%2%) is unknwon"} % __func__ % (detail::ntoh(header.type))).str()};
            // TODO return {};
        }
    }

    namespace detail {

        struct to_any_action
        {
            template <class Action>
            auto operator()(Action&& action) const
                -> any_action
            {
                return any_action{std::forward<Action>(action)};
            }
        };

    } // namespace detail

    template <class Iterator>
    auto decode_action(Iterator& first, Iterator last)
        -> any_action
    {
        return decode_action<any_action>(first, last, detail::to_any_action{});
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTIONS_HPP
