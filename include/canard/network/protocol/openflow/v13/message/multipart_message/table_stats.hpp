#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {

    class table_stats
    {
    public:
        static constexpr std::size_t base_size
            = sizeof(v13_detail::ofp_table_stats);

        table_stats(std::uint8_t const table_id
                  , std::uint32_t const active_count
                  , std::uint64_t const lookup_count
                  , std::uint64_t const matched_count) noexcept
            : table_stats_{
                  table_id
                , { 0, 0, 0 }
                , active_count
                , lookup_count
                , matched_count
              }
        {
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(v13_detail::ofp_table_stats);
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return table_stats_.table_id;
        }

        auto active_count() const noexcept
            -> std::uint32_t
        {
            return table_stats_.active_count;
        }

        auto lookup_count() const noexcept
            -> std::uint64_t
        {
            return table_stats_.lookup_count;
        }

        auto matched_count() const noexcept
            -> std::uint64_t
        {
            return table_stats_.matched_count;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, table_stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_stats
        {
            return table_stats{
                detail::decode<v13_detail::ofp_table_stats>(first, last)
            };
        }

    private:
        explicit table_stats(v13_detail::ofp_table_stats const& table_stats)
            : table_stats_(table_stats)
        {
        }

    private:
        v13_detail::ofp_table_stats table_stats_;
    };


    class table_stats_request
        : public multipart_detail::basic_multipart_request<
                table_stats_request, void
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_TABLE;

        explicit table_stats_request(
                std::uint32_t const xid = get_xid()) noexcept
            : basic_multipart_request{0, xid}
        {
        }

    private:
        friend basic_multipart_request::base_type;

        explicit table_stats_request(
                v13_detail::ofp_multipart_request const& multipart_request) noexcept
            : basic_multipart_request{multipart_request}
        {
        }
    };


    class table_stats_reply
        : public multipart_detail::basic_multipart_reply<
                table_stats_reply, table_stats[]
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_TABLE;

        explicit table_stats_reply(
                  body_type table_stats
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_multipart_reply{flags, std::move(table_stats), xid}
        {
        }

    private:
        friend basic_multipart_reply::base_type;

        table_stats_reply(
                  v13_detail::ofp_multipart_reply const& reply
                , body_type&& table_stats)
            : basic_multipart_reply{reply, std::move(table_stats)}
        {
        }
    };

} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_STATS_HPP
