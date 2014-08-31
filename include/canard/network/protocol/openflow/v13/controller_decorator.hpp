#ifndef CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP

#include <utility>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class null_decorator
    {
    public:
        template <class Derived>
        struct type
        {
            template <class... Args>
            void handle(Args&&... args)
            {
                static_cast<Derived*>(this)->handle(std::forward<Args>(args)...);
            }
        };
    };

    template <class Derived, class Decorators>
    class decoration
        : public Decorators::template type<Derived>
    {
    public:
        using base_type = decoration;

        template <class... Args>
        decoration(Args&&... args)
            : Decorators::template type<Derived>{std::forward<Args>(args)...}
        {
        }
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CONTROLLER_DECORATOR_HPP
