#ifndef CANARD_NETWORK_OPENFLOW_MIN_BASE_SIZE_ELEMENT_HPP
#define CANARD_NETWORK_OPENFLOW_MIN_BASE_SIZE_ELEMENT_HPP

#include <cstddef>
#include <type_traits>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    struct min_base_size
    {
        template <class LHS, class RHS>
        struct apply
        {
            using type = typename std::conditional<
                (LHS::base_size < RHS::base_size), LHS, RHS
            >::type;
        };

        template <class LHS, class RHS>
        auto operator()(LHS, RHS) const
            -> typename apply<LHS, RHS>::type;
    };

    template <class Sequence>
    struct min_base_size_element
    {
        using head = typename boost::fusion::result_of::value_at_c<
            Sequence, 0
        >::type;

        using type = typename boost::fusion::result_of::accumulate<
            Sequence, head, min_base_size
        >::type;

        static constexpr std::size_t value = type::base_size;
    };

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_MIN_BASE_SIZE_ELEMENT_HPP
