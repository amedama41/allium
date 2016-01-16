#ifndef CANARD_NETWORK_OPENFLOW_DATA_PER_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_DATA_PER_CHANNEL_HPP

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        struct null_data {};

    } // namespace detail

    template <class ControllerHandler>
    struct data_per_channel
    {
        using type = detail::null_data;
    };

    template <class ControllerHandler>
    using data_per_channel_t
        = typename data_per_channel<ControllerHandler>::type;

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DATA_PER_CHANNEL_HPP
