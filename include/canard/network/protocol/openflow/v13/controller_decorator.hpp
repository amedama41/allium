#ifndef CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP

#include <utility>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class null_controller
    {
    public:
        template <class Derived>
        struct type
        {
            template <class... Args>
            void connected(Args&&... args)
            {
                static_cast<Derived*>(this)->connected(std::forward<Args>(args)...);
            }

            template <class... Args>
            void disconnected(Args&&... args)
            {
                static_cast<Derived*>(this)->disconnected(std::forward<Args>(args)...);
            }

            template <class... Args>
            void handle(Args&&... args)
            {
                static_cast<Derived*>(this)->handle(std::forward<Args>(args)...);
            }
        };
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP
