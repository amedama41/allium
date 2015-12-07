#ifndef CANARD_NETWORK_OPENFLOW_V13_DUMMY_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DUMMY_HANDLER_HPP

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        struct dummy_handler
        {
            template <class... Args>
            void operator()(Args&&...) const
            {
            }
        };

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DUMMY_HANDLER_HPP
