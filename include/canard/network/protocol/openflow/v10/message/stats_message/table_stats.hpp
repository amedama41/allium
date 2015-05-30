#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_TABLE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_TABLE_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/stats_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

namespace stats {

    class table_stats
    {
    public:
        static std::uint16_t const base_size = sizeof(v10_detail::ofp_table_stats);

        auto table_id() const
            -> std::uint8_t
        {
            return table_stats_.table_id;
        }

        auto name() const
            -> boost::string_ref
        {
            return table_stats_.name;
        }

        auto wildcards() const
            -> std::uint32_t
        {
            return table_stats_.wildcards;
        }

        auto max_entries() const
            -> std::uint32_t
        {
            return table_stats_.max_entries;
        }

        auto active_count() const
            -> std::uint32_t
        {
            return table_stats_.active_count;
        }

        auto lookup_count() const
            -> std::uint32_t
        {
            return table_stats_.lookup_count;
        }

        auto matched_count() const
            -> std::uint32_t
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

} // namespace stats

namespace messages {

    class table_stats_request
        : public v10_detail::stats_request_adaptor<
                table_stats_request, void
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_TABLE;

        explicit table_stats_request(std::uint32_t const xid = get_xid())
            : stats_request_adaptor{xid}
        {
        }

    private:
        friend stats_request_adaptor;

        explicit table_stats_request(
                v10_detail::ofp_stats_request const& stats_request)
            : stats_request_adaptor{stats_request}
        {
        }
    };


    class table_stats_reply
        : public v10_detail::stats_reply_adaptor<
                table_stats_reply, stats::table_stats, true
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_TABLE;

        using iterator = std::vector<stats::table_stats>::const_iterator;
        using const_iterator = std::vector<stats::table_stats>::const_iterator;

        auto num_tables() const
            -> std::uint8_t
        {
            return table_stats_.size();
        }

        auto begin() const
            -> const_iterator
        {
            return table_stats_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return table_stats_.end();
        }

    private:
        friend stats_reply_adaptor;

        auto body() const
            -> std::vector<stats::table_stats> const&
        {
            return table_stats_;
        }

        table_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , std::vector<stats::table_stats> table_stats)
            : stats_reply_adaptor{stats_reply}
            , table_stats_(std::move(table_stats))
        {
        }

    private:
        std::vector<stats::table_stats> table_stats_;
    };

} // namespace messages

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_TABLE_STATS_HPP
