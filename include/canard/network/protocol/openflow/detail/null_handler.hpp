#ifndef CANARD_NETWORK_OPENFLOW_NULL_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_NULL_HANDLER_HPP

namespace canard {
namespace net {
namespace ofp {
namespace controller {
namespace detail {

    class null_handler
    {
    public:
        template <class... Args>
        void operator()(Args&&...) const
        {
        }
    };

} // namespace detail
} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_NULL_HANDLER_HPP
