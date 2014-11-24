#ifndef CANARD_NETWORK_OPENFLOW_GET_XID_HPP
#define CANARD_NETWORK_OPENFLOW_GET_XID_HPP

#include <cstdint>
#include <atomic>

namespace canard {
namespace network {
namespace openflow {

    inline auto get_xid()
        -> std::uint32_t
    {
        static std::atomic<std::uint32_t> xid{0};
        return xid.fetch_add(1, std::memory_order_relaxed);
    }

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_GET_XID_HPP
