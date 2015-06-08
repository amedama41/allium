#ifndef CANARD_NETWORK_OPENFLOW_MIN_RAW_SIZE_HPP
#define CANARD_NETWORK_OPENFLOW_MIN_RAW_SIZE_HPP

#include <cstdint>
#include <type_traits>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    struct raw_size_min
    {
        template <class LHS, class RHS>
        struct apply
        {
            using type = typename std::conditional<
                (LHS::raw_size < RHS::raw_size), LHS, RHS
            >::type;
        };

        template <class LHS, class RHS>
        auto operator()(LHS, RHS) const
            -> typename apply<LHS, RHS>::type;
    };

    template <class HaveRawSizeElements>
    struct raw_size_min_element
    {
        using first_type = typename boost::fusion::result_of::value_at_c<
            HaveRawSizeElements, 0
        >::type;

        using type = typename boost::fusion::result_of::accumulate<
            HaveRawSizeElements, first_type, raw_size_min
        >::type;

        static std::uint16_t const value = type::raw_size;
    };

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_MIN_RAW_SIZE_HPP
