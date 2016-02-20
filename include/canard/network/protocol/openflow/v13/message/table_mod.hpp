#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_TABLE_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_TABLE_MOD_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class table_mod
        : public v13_detail::basic_openflow_message<table_mod>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_TABLE_MOD;

        table_mod(std::uint8_t const table_id
                , std::uint32_t const config
                , std::uint32_t const xid = get_xid())
            : table_mod_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , sizeof(v13_detail::ofp_table_mod)
                    , xid
                  }
                , table_id
                , { 0, 0, 0 }
                , config
              }
        {
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
        {
            return table_mod_.header;
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return table_mod_.table_id;
        }

        auto config() const noexcept
            -> std::uint32_t
        {
            return table_mod_.config;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, table_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_mod
        {
            return table_mod{
                detail::decode<v13_detail::ofp_table_mod>(first, last)
            };
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length != sizeof(v13_detail::ofp_table_mod)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit table_mod(v13_detail::ofp_table_mod const table_mod) noexcept
            : table_mod_(table_mod)
        {
        }

    private:
        v13_detail::ofp_table_mod table_mod_;
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_TABLE_MOD_HPP
