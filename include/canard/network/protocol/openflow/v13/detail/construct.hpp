#ifndef CANARD_NETWORK_OPENFLOW_V13_CONSTRUCT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CONSTRUCT_HPP

#include <utility>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

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

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CONSTRUCT_HPP
