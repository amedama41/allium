#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_NULL_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_NULL_HANDLER_HPP

namespace canard {
namespace network {
namespace openflow {
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
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_NULL_HANDLER_HPP
