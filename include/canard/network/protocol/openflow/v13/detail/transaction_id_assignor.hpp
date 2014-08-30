#ifndef CANARD_NETWORK_OPENFLOW_V13_TRANSACTION_ID_ASSIGNOR
#define CANARD_NETWORK_OPENFLOW_V13_TRANSACTION_ID_ASSIGNOR

#include <cstdint>
#include <atomic>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class Unsigned = std::uint32_t>
        class transaction_id_assignor
        {
        protected:
            static auto get_xid()
                -> Unsigned
            {
                return xid.fetch_add(1, std::memory_order_relaxed);
            }

        private:
            static std::atomic<Unsigned> xid;
        };

        template <class Unsigned>
        std::atomic<Unsigned> transaction_id_assignor<Unsigned>::xid{0};

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TRANSACTION_ID_ASSIGNOR
