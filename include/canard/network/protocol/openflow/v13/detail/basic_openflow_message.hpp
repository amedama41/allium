#ifndef CANARD_NETWORK_OPENFLOW_V13_BASIC_OPENFLOW_MESSAGE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_BASIC_OPENFLOW_MESSAGE_HPP

#include <cstdint>
#include <vector>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/detail/transaction_id_assignor.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <class T>
        class basic_openflow_message
            : public v13::detail::transaction_id_assignor<std::uint32_t>
        {
        private:
            auto header() const
                -> v13_detail::ofp_header const&
            {
                return static_cast<T const*>(this)->header();
            }

        public:
            auto version() const
                -> std::uint8_t
            {
                return header().version;
            }

            auto type() const
                -> protocol::ofp_type
            {
                return protocol::ofp_type(header().type);
            }

            auto length() const
                -> std::uint16_t
            {
                return header().length;
            }

            auto xid() const
                -> std::uint32_t
            {
                return header().xid;
            }

            auto encode() const
                -> std::vector<unsigned char>
            {
                auto buf = std::vector<unsigned char>{};
                buf.reserve(length());
                return static_cast<T const*>(this)->encode(buf);
            }
        };

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_BASIC_OPENFLOW_MESSAGE_HPP
