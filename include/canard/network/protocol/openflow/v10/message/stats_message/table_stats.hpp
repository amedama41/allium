#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_TABLE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_TABLE_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_stats.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace statistics {

    class table_stats
    {
    public:
        static constexpr std::size_t base_size
            = sizeof(v10_detail::ofp_table_stats);

        table_stats(std::uint8_t const table_id
                  , boost::string_ref name
                  , std::uint32_t const wildcards
                  , std::uint32_t const max_entries
                  , std::uint32_t const active_count
                  , std::uint64_t const lookup_count
                  , std::uint64_t const matched_count)
            : table_stats_{
                  table_id
                , { 0, 0, 0 }
                , { 0 }
                , wildcards
                , max_entries
                , active_count
                , lookup_count
                , matched_count
              }
        {
            auto const name_size
                = std::min(name.size(), sizeof(table_stats_.name) - 1);
            using boost::adaptors::sliced;
            boost::copy(name | sliced(0, name_size), table_stats_.name);
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(v10_detail::ofp_table_stats);
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return table_stats_.table_id;
        }

        auto name() const
            -> boost::string_ref
        {
            return table_stats_.name;
        }

        auto wildcards() const noexcept
            -> std::uint32_t
        {
            return table_stats_.wildcards;
        }

        auto max_entries() const noexcept
            -> std::uint32_t
        {
            return table_stats_.max_entries;
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
                detail::decode<v10_detail::ofp_table_stats>(first, last)
            };
        }

    private:
        explicit table_stats(v10_detail::ofp_table_stats const& table_stats)
            : table_stats_(table_stats)
        {
        }

    private:
        v10_detail::ofp_table_stats table_stats_;
    };


    class table_stats_request
        : public stats_detail::basic_stats_request<
                table_stats_request, void
          >
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_TABLE;

        explicit table_stats_request(
                std::uint32_t const xid = get_xid()) noexcept
            : basic_stats_request{0, xid}
        {
        }

    private:
        friend basic_stats_request::base_type;

        explicit table_stats_request(
                v10_detail::ofp_stats_request const& stats_request) noexcept
            : basic_stats_request{stats_request}
        {
        }
    };


    class table_stats_reply
        : public stats_detail::basic_stats_reply<
                table_stats_reply, table_stats[]
          >
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_TABLE;

        explicit table_stats_reply(
                  body_type table_stats
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_stats_reply{flags, std::move(table_stats), xid}
        {
        }

    private:
        friend basic_stats_reply::base_type;

        table_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , body_type&& table_stats)
            : basic_stats_reply{stats_reply, std::move(table_stats)}
        {
        }
    };

} // namespace statistics
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_TABLE_STATS_HPP
