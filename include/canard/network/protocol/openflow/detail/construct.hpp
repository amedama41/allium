#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_CONSTRUCT_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_CONSTRUCT_HPP

#include <utility>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    template <class To>
    struct construct
    {
        template <class T>
        auto operator()(T&& t) const
            -> To
        {
            return To(std::forward<T>(t));
        }
    };

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_CONSTRUCT_HPP
