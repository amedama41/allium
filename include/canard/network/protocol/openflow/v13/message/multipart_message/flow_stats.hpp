#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_STATS_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class flow_stats
    {
    public:
        auto entry() const
            -> flow_entry const&
        {
            return entry_;
        }

        auto table_id() const
            -> std::uint8_t
        {
            return table_id_;
        }

        auto flags() const
            -> std::uint16_t
        {
            return flags_;
        }

        auto duration_sec() const
            -> std::uint32_t
        {
            return duration_sec_;
        }

        auto duration_nsec() const
            -> std::uint32_t
        {
            return duration_nsec_;
        }

        auto length() const
            -> std::uint16_t
        {
            return detail::exact_length(sizeof(v13_detail::ofp_flow_stats)
                    + entry().match().length() + entry().instructions().length());
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, v13_detail::ofp_flow_stats{
                      length()
                    , table_id_
                    , 0
                    , duration_sec_
                    , duration_nsec_
                    , entry().priority()
                    , entry().idle_timeout()
                    , entry().hard_timeout()
                    , flags_
                    , {0, 0, 0, 0}
                    , entry().cookie()
                    , entry().packet_count()
                    , entry().byte_count()
            });
            entry().match().encode(container);
            return entry().instructions().encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats
        {
            auto const stats = detail::decode<v13_detail::ofp_flow_stats>(first, last);
            last = std::next(first, stats.length - sizeof(v13_detail::ofp_flow_stats));
            auto match = oxm_match::decode(first, last);
            auto instructions = instruction_set::decode(first, last);

            return {stats, std::move(match), std::move(instructions)};
        }

    private:
        flow_stats(v13_detail::ofp_flow_stats const& stats, oxm_match match, instruction_set instructions)
            : entry_{
                  {std::move(match), stats.priority}
                , std::move(instructions)
                , {stats.packet_count, stats.byte_count}
                , {stats.idle_timeout, stats.hard_timeout}
                , stats.cookie
              }
            , duration_sec_(stats.duration_sec)
            , duration_nsec_(stats.duration_nsec)
            , flags_(stats.flags)
            , table_id_(stats.table_id)
        {
        }

    private:
        flow_entry entry_;
        std::uint32_t duration_sec_;
        std::uint32_t duration_nsec_;
        std::uint16_t flags_;
        std::uint8_t table_id_;
    };


namespace messages {

    class flow_stats_request
        : public v13_detail::basic_multipart_request<flow_stats_request>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_FLOW;

        explicit flow_stats_request(oxm_match match)
            : basic_multipart_request{
                  detail::exact_length(sizeof(v13_detail::ofp_flow_stats_request) + match.length())
                , 0
              }
            , flow_stats_request_{
                  protocol::OFPTT_ALL, {0, 0, 0}
                , protocol::OFPP_ANY, protocol::OFPG_ANY
                , {0, 0, 0, 0}, 0, 0
              }
            , match_(std::move(match))
        {
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_request::encode(container);
            detail::encode(container, flow_stats_request_);
            return match_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats_request
        {
            auto const request = basic_multipart_request::decode(first, last);
            auto const stats_reqeust = detail::decode<v13_detail::ofp_flow_stats_request>(first, last);
            auto match = oxm_match::decode(first, last);
            return flow_stats_request{request, stats_reqeust, std::move(match)};
        }

    private:
        flow_stats_request(v13_detail::ofp_multipart_request const& request
                , v13_detail::ofp_flow_stats_request const& stats_request, oxm_match match)
            : basic_multipart_request{request}
            , flow_stats_request_(stats_request)
            , match_(std::move(match))
        {
        }

    private:
        v13_detail::ofp_flow_stats_request flow_stats_request_;
        oxm_match match_;
    };


    class flow_stats_reply
        : public v13_detail::basic_multipart_reply<flow_stats_reply>
    {
        using flow_stats_list = std::vector<flow_stats>;

    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_FLOW;

        using const_iterator = flow_stats_list::const_iterator;

        auto begin() const
            -> const_iterator
        {
            return flow_stats_list_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return flow_stats_list_.end();
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_reply::encode(container);
            boost::for_each(flow_stats_list_, [&](flow_stats const& stats) {
                stats.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats_reply
        {
            auto reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(v13_detail::ofp_multipart_reply)) {
                throw 2;
            }

            auto stats_list = std::vector<flow_stats>{};
            stats_list.reserve(std::distance(first, last) / (sizeof(v13_detail::ofp_flow_stats) + sizeof(v13_detail::ofp_match)));
            while (first != last) {
                stats_list.push_back(flow_stats::decode(first, last));
            }

            return flow_stats_reply{reply, std::move(stats_list)};
        }

    private:
        flow_stats_reply(v13_detail::ofp_multipart_reply const& reply, flow_stats_list stats_list)
            : basic_multipart_reply{reply}
            , flow_stats_list_(std::move(stats_list))
        {
        }

    private:
        flow_stats_list flow_stats_list_;
    };

} // namespace messages

using messages::flow_stats_request;
using messages::flow_stats_reply;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_STATS_HPP
// vim: path+=../../
