#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_STATS_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class table_stats
    {
    public:
        explicit table_stats(std::uint8_t const table_id)
            : table_stats_{table_id, {0, 0, 0}, 0, 0, 0}
        {
        }

        table_stats(std::uint8_t const table_id
                , std::uint32_t const active_count
                , std::uint64_t const lookup_count
                , std::uint64_t const matched_count)
            : table_stats_{table_id, {0, 0, 0}, active_count, lookup_count, matched_count}
        {
        }

        auto table_id() const
            -> std::uint8_t
        {
            return table_stats_.table_id;
        }

        auto active_count() const
            -> std::uint32_t
        {
            return table_stats_.active_count;
        }

        auto lookup_count() const
            -> std::uint64_t
        {
            return table_stats_.lookup_count;
        }

        auto matched_count() const
            -> std::uint64_t
        {
            return table_stats_.matched_count;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return v13_detail::encode(container, table_stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_stats
        {
            return table_stats{v13_detail::decode<v13_detail::ofp_table_stats>(first, last)};
        }

    private:
        explicit table_stats(v13_detail::ofp_table_stats const& table_stats)
            : table_stats_(table_stats)
        {
        }

    private:
        v13_detail::ofp_table_stats table_stats_;
    };

namespace messages {

    class table_stats_request
        : public v13_detail::basic_multipart_request<table_stats_request>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_TABLE;

        table_stats_request()
            : basic_multipart_request{0, 0}
        {
        }
    };


    class table_stats_reply
        : public v13_detail::basic_multipart_reply<table_stats_reply>
    {
        using table_stats_list = std::vector<table_stats>;
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_TABLE;

        using iterator = table_stats_list::iterator;
        using const_iterator = table_stats_list::const_iterator;

        auto begin() const
            -> const_iterator
        {
            return table_stats_list_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return table_stats_list_.end();
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_stats_reply
        {
            auto const reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(v13_detail::ofp_multipart_reply)) {
                throw 2;
            }

            auto stats_list = table_stats_list{};
            stats_list.reserve(std::distance(first, last) / sizeof(v13_detail::ofp_table_stats));
            while (first != last) {
                stats_list.push_back(table_stats::decode(first, last));
            }
            return {reply, std::move(stats_list)};
        }

    private:
        table_stats_reply(v13_detail::ofp_multipart_reply const& reply, table_stats_list stats_list)
            : basic_multipart_reply{reply}
            , table_stats_list_(std::move(stats_list))
        {
        }

    private:
        table_stats_list table_stats_list_;
    };

} // namespace messages

using messages::table_stats_request;
using messages::table_stats_reply;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_STATS_HPP
