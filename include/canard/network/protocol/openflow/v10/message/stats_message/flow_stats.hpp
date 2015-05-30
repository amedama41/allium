#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_STATS_HPP

#include <cstdint>
#include <chrono>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/basic_stats_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

namespace stats {

    class flow_stats
        : public v10_detail::flow_entry_adaptor<
              flow_stats, v10_detail::ofp_flow_stats
          >
    {
    public:
        auto length() const
            -> std::uint16_t
        {
            return flow_stats_.length;
        }

        auto table_id() const
            -> std::uint8_t
        {
            return flow_stats_.table_id;
        }

        auto duration_sec() const
            -> std::uint32_t
        {
            return flow_stats_.duration_sec;
        }

        auto duration_nsec() const
            -> std::uint32_t
        {
            return flow_stats_.duration_nsec;
        }

        auto actions() const
            -> action_list const&
        {
            return actions_;
        }

        auto ofp_flow_entry() const
            -> v10_detail::ofp_flow_stats const&
        {
            return flow_stats_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return actions_.encode(detail::encode(container, flow_stats_));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats
        {
            auto const stats
                = detail::decode<v10_detail::ofp_flow_stats>(first, last);
            if (stats.length > sizeof(stats) + std::distance(first, last)) {
                throw std::runtime_error{"invalid flow entry length"};
            }
            auto actions = action_list::decode(
                    first, std::next(first, stats.length - sizeof(stats)));
            return flow_stats{stats, std::move(actions)};
        }

    private:
        flow_stats(v10_detail::ofp_flow_stats const& stats, action_list actions)
            : flow_stats_(stats), actions_(std::move(actions))
        {
        }

    private:
        v10_detail::ofp_flow_stats flow_stats_;
        action_list actions_;
    };

} // namespace stats

namespace messages {

    class flow_stats_request
        : public messages_detail::basic_stats_request<
                flow_stats_request, v10_detail::ofp_stats_request
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_FLOW;

        explicit flow_stats_request(
                  match_set const& match
                , std::uint32_t const xid = get_xid())
            : flow_stats_request{match, 0xff, OFPP_NONE, xid}
        {
        }

        flow_stats_request(
                  match_set const& match
                , std::uint8_t const table_id, std::uint16_t const out_port
                , std::uint32_t const xid = get_xid())
            : basic_stats_request{xid, sizeof(flow_stats_request_)}
            , flow_stats_request_{match.ofp_match(), table_id, 0, out_port}
        {
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_stats_request::encode(container);
            return detail::encode(container, flow_stats_request_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats_request
        {
            auto const stats_request = basic_stats_request::decode(first, last);
            auto const flow_stats
                = detail::decode<v10_detail::ofp_flow_stats_request>(first, last);
            return flow_stats_request{stats_request, flow_stats};
        }

    private:
        flow_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_flow_stats_request const& flow_stats)
            : basic_stats_request{stats_request, sizeof(flow_stats_request_)}
            , flow_stats_request_(flow_stats)
        {
        }

    private:
        v10_detail::ofp_flow_stats_request flow_stats_request_;
    };


    namespace flow_stats_detail {

        static inline auto num_of_flow_stats(
                std::uint16_t const flow_stats_reply_length)
            -> std::size_t
        {
            return flow_stats_reply_length / sizeof(v10_detail::ofp_flow_stats);
        }

        static inline auto flow_stats_length(
                std::vector<stats::flow_stats> const& stats_list)
            -> std::uint16_t
        {
            return boost::accumulate(
                      stats_list, std::uint16_t{0}
                    , [](std::uint16_t const len, stats::flow_stats const& st) {
                            return len + st.length();
                      });
        }

    } // namespace flow_stats_detail

    class flow_stats_reply

        : public messages_detail::basic_stats_reply<
                flow_stats_reply, v10_detail::ofp_stats_reply
          >
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_FLOW;

        using iterator = std::vector<stats::flow_stats>::const_iterator;
        using const_iterator = std::vector<stats::flow_stats>::const_iterator;

        auto num_flows() const
            -> std::size_t
        {
            return flow_stats_.size();
        }

        auto begin() const
            -> const_iterator
        {
            return flow_stats_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return flow_stats_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_stats_reply::encode(container);
            boost::for_each(flow_stats_, [&](stats::flow_stats const& stats) {
                stats.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats_reply
        {
            auto const stats_reply = basic_stats_reply::decode(first, last);

            auto flow_stats = std::vector<stats::flow_stats>{};
            flow_stats.reserve(
                    flow_stats_detail::num_of_flow_stats(stats_reply.header.length));
            while (first != last) {
                flow_stats.emplace_back(stats::flow_stats::decode(first, last));
            }

            return flow_stats_reply{stats_reply, std::move(flow_stats)};
        }

    private:
        flow_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , std::vector<stats::flow_stats> flow_stats)
            : basic_stats_reply{
                stats_reply, flow_stats_detail::flow_stats_length(flow_stats)
              }
            , flow_stats_(std::move(flow_stats))
        {
        }

    private:
        std::vector<stats::flow_stats> flow_stats_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_STATS_HPP
