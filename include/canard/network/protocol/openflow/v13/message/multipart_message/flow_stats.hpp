#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_FLOW_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_FLOW_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {

    class flow_stats
        : public v13_detail::flow_entry_adaptor<
                flow_stats, v13_detail::ofp_flow_stats
          >
    {
    public:
        static constexpr std::size_t base_size
            = sizeof(v13_detail::ofp_flow_stats)
            + sizeof(v13_detail::ofp_match);

        flow_stats(v13::flow_entry entry
                 , std::uint8_t const table_id
                 , std::uint16_t const flags
                 , v13::timeouts const& timeouts
                 , v13::elapsed_time const& elapsed_time
                 , v13::counters const& counters)
            : flow_stats_{
                  std::uint16_t(
                          sizeof(v13_detail::ofp_flow_stats)
                        + v13_detail::exact_length(entry.match().length())
                        + entry.instructions().length())
                , table_id
                , 0
                , elapsed_time.duration_sec()
                , elapsed_time.duration_nsec()
                , entry.priority()
                , timeouts.idle_timeout()
                , timeouts.hard_timeout()
                , flags
                , { 0, 0, 0, 0 }
                , entry.cookie()
                , counters.packet_count()
                , counters.byte_count()
              }
            , match_(std::move(entry).match())
            , instructions_(std::move(entry).instructions())
        {
        }

        flow_stats(flow_stats const&) = default;

        flow_stats(flow_stats&& other)
            : flow_stats_(other.flow_stats_)
            , match_(std::move(other).match_)
            , instructions_(std::move(other).instructions_)
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
            match_.swap(other.match_);
            instructions_.swap(other.instructions_);
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return flow_stats_.length;
        }

        auto match() const noexcept
            -> oxm_match_set const&
        {
            return match_;
        }

        auto instructions() const noexcept
            -> instruction_set const&
        {
            return instructions_;
        }

        auto entry() const
            -> flow_entry
        {
            return flow_entry{match(), priority(), cookie(), instructions()};
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_stats_);
            match_.encode(container);
            return instructions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_stats
        {
            auto const stats
                = detail::decode<v13_detail::ofp_flow_stats>(first, last);
            if (stats.length < base_size) {
                throw std::runtime_error{"flow_stats length is too small"};
            }
            if (std::distance(first, last) + sizeof(v13_detail::ofp_flow_stats)
                    < stats.length) {
                throw std::runtime_error{"flow_stats length is too big"};
            }
            last = std::next(
                    first, stats.length - sizeof(v13_detail::ofp_flow_stats));

            auto copy_first = first;
            auto const ofp_match
                = detail::decode<v13_detail::ofp_match>(copy_first, last);
            oxm_match_set::validate(ofp_match);
            if (std::distance(first, last)
                    < v13_detail::exact_length(ofp_match.length)) {
                throw std::runtime_error{"invalid oxm_match length"};
            }
            auto match = oxm_match_set::decode(first, last);

            auto instructions = instruction_set::decode(first, last);

            return flow_stats{stats, std::move(match), std::move(instructions)};
        }

    private:
        flow_stats(v13_detail::ofp_flow_stats const& stats
                 , oxm_match_set&& match
                 , instruction_set&& instructions)
            : flow_stats_(stats)
            , match_(std::move(match))
            , instructions_(std::move(instructions))
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v13_detail::ofp_flow_stats const&
        {
            return flow_stats_;
        }

    private:
        v13_detail::ofp_flow_stats flow_stats_;
        oxm_match_set match_;
        instruction_set instructions_;
    };


    class flow_stats_request
        : public multipart_detail::basic_multipart_request<
              flow_stats_request, v13_detail::ofp_flow_stats_request, true
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_FLOW;

        flow_stats_request(
                  flow_entry entry
                , std::uint8_t const table_id
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : basic_multipart_request{
                  0
                , v13_detail::ofp_flow_stats_request{
                      table_id
                    , { 0, 0, 0 }
                    , out_port
                    , out_group
                    , { 0, 0, 0, 0 }
                    , entry.cookie()
                    , std::numeric_limits<std::uint64_t>::max()
                  }
                , std::move(std::move(entry).match())
                , xid
              }
        {
        }

        flow_stats_request(
                  oxm_match_set match
                , std::uint8_t const table_id
                , v13::cookie_mask const& cookie_mask
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : basic_multipart_request{
                  0
                , v13_detail::ofp_flow_stats_request{
                      table_id
                    , { 0, 0, 0 }
                    , out_port
                    , out_group
                    , { 0, 0, 0, 0 }
                    , cookie_mask.value()
                    , cookie_mask.mask()
                  }
                , std::move(match)
                , xid
              }
        {
        }

        flow_stats_request(
                  oxm_match_set match
                , std::uint8_t const table_id
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : flow_stats_request{
                  std::move(match)
                , table_id
                , v13::cookie_mask{0, 0}
                , out_port
                , out_group
                , xid
              }
        {
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return body().table_id;
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return body().cookie;
        }

        auto cookie_mask() const noexcept
            -> std::uint64_t
        {
            return body().cookie_mask;
        }

        auto out_port() const noexcept
            -> std::uint32_t
        {
            return body().out_port;
        }

        auto out_group() const noexcept
            -> std::uint32_t
        {
            return body().out_group;
        }

    private:
        friend basic_multipart_request::base_type;

        flow_stats_request(
                  v13_detail::ofp_multipart_request const& request
                , v13_detail::ofp_flow_stats_request const& stats_request
                , oxm_match_set&& match)
            : basic_multipart_request{request, stats_request, std::move(match)}
        {
        }
    };


    class flow_stats_reply
        : public multipart_detail::basic_multipart_reply<
              flow_stats_reply, flow_stats[]
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_FLOW;

        explicit flow_stats_reply(
                  body_type flow_stats
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_multipart_reply{flags, std::move(flow_stats), xid}
        {
        }

    private:
        friend basic_multipart_reply::base_type;

        flow_stats_reply(
                  v13_detail::ofp_multipart_reply const& multipart_reply
                , body_type&& flow_stats)
            : basic_multipart_reply{multipart_reply, std::move(flow_stats)}
        {
        }
    };

} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_FLOW_STATS_HPP
