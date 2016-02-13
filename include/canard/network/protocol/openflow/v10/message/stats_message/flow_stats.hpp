#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_FLOW_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_FLOW_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_stats.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace statistics {

    class flow_stats
        : public v10_detail::flow_entry_adaptor<
                flow_stats, v10_detail::ofp_flow_stats
          >
    {
    public:
        static constexpr std::uint16_t base_size
            = sizeof(v10_detail::ofp_flow_stats);

        flow_stats(v10::flow_entry entry
                 , std::uint8_t const table_id
                 , v10::timeouts const& timeouts
                 , v10::elapsed_time const& elapsed_time
                 , v10::counters const& counters)
            : flow_stats_{
                  std::uint16_t(
                          sizeof(v10_detail::ofp_flow_stats)
                        + entry.actions().length())
                , table_id
                , 0
                , entry.match().ofp_match()
                , elapsed_time.duration_sec()
                , elapsed_time.duration_nsec()
                , entry.priority()
                , timeouts.idle_timeout()
                , timeouts.hard_timeout()
                , { 0, 0, 0, 0, 0, 0 }
                , entry.cookie()
                , counters.packet_count()
                , counters.byte_count()
              }
            , actions_(std::move(entry).actions())
        {
        }

        flow_stats(flow_stats const&) = default;

        flow_stats(flow_stats&& other)
            : flow_stats_(other.flow_stats_)
            , actions_(std::move(other).actions_)
        {
            other.flow_stats_.length = base_size;
        }

        auto operator=(flow_stats const&)
            -> flow_stats& = default;

        auto operator=(flow_stats&& other)
            -> flow_stats&
        {
            auto tmp = std::move(other);
            swap(tmp);
            return *this;
        }

        void swap(flow_stats& other)
        {
            std::swap(flow_stats_, other.flow_stats_);
            actions_.swap(other.actions_);
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return flow_stats_.length;
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return flow_stats_.table_id;
        }

        auto actions() const noexcept
            -> action_list const&
        {
            return actions_;
        }

        auto entry() const
            -> flow_entry
        {
            return flow_entry{match(), priority(), cookie(), actions()};
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_stats_);
            return actions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats
        {
            auto const stats
                = detail::decode<v10_detail::ofp_flow_stats>(first, last);
            if (std::distance(first, last) + base_size < stats.length) {
                throw std::runtime_error{"invalid flow_stats length"};
            }
            last = std::next(first, stats.length - base_size);

            auto actions = action_list::decode(first, last);

            return flow_stats{stats, std::move(actions)};
        }

    private:
        flow_stats(v10_detail::ofp_flow_stats const& stats
                 , action_list&& actions)
            : flow_stats_(stats)
            , actions_(std::move(actions))
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v10_detail::ofp_flow_stats const&
        {
            return flow_stats_;
        }

    private:
        v10_detail::ofp_flow_stats flow_stats_;
        action_list actions_;
    };


    class flow_stats_request
        : public stats_detail::basic_stats_request<
                flow_stats_request, v10_detail::ofp_flow_stats_request
          >
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_FLOW;

        flow_stats_request(
                  match_set const& match
                , std::uint8_t const table_id
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid()) noexcept
            : basic_stats_request{
                  0
                , v10_detail::ofp_flow_stats_request{
                      match.ofp_match()
                    , table_id
                    , 0
                    , out_port
                  }
                , xid
              }
        {
        }

        auto match() const noexcept
            -> match_set
        {
            return match_set{body().match};
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return body().table_id;
        }

        auto out_port() const noexcept
            -> std::uint16_t
        {
            return body().out_port;
        }

    private:
        friend basic_stats_request::base_type;

        flow_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , v10_detail::ofp_flow_stats_request const& flow_stats) noexcept
            : basic_stats_request{stats_request, flow_stats}
        {
        }
    };


    class flow_stats_reply
        : public stats_detail::basic_stats_reply<flow_stats_reply, flow_stats[]>
    {
    public:
        static constexpr protocol::ofp_stats_types stats_type_value
            = protocol::OFPST_FLOW;

        explicit flow_stats_reply(
                  body_type flow_stats
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_stats_reply{flags, std::move(flow_stats), xid}
        {
        }

    private:
        friend basic_stats_reply::base_type;

        flow_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , body_type&& flow_stats)
            : basic_stats_reply{stats_reply, std::move(flow_stats)}
        {
        }
    };

} // namespace statistics
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATISTICS_FLOW_STATS_HPP
