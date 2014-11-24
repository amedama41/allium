#ifndef CANARD_NETWORK_OPENFLOW_ADD_HELPER_HPP
#define CANARD_NETWORK_OPENFLOW_ADD_HELPER_HPP

#include <utility>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class Container>
        struct add_helper
        {
            template <class T>
            void operator()(T&& t) const
            {
                container.add(std::forward<T>(t));
            }

            Container& container;
        };

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_ADD_HELPER_HPP
