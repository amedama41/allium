#ifndef CANARD_NETWORK_OPENFLOW_QUEUE_ID_HPP
#define CANARD_NETWORK_OPENFLOW_QUEUE_ID_HPP

#include <cstdint>

namespace canard {
namespace network {
namespace openflow {

    class queue_id
    {
    public:
        constexpr queue_id(
                  std::uint32_t const queue_id
                , std::uint32_t const port_no) noexcept
            : queue_id_(queue_id)
            , port_no_(port_no)
        {
        }

        constexpr auto queue() const noexcept
            -> std::uint32_t
        {
            return queue_id_;
        }

        constexpr auto port() const noexcept
            -> std::uint32_t
        {
            return port_no_;
        }

    private:
        std::uint32_t queue_id_;
        std::uint32_t port_no_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_QUEUE_ID_HPP
