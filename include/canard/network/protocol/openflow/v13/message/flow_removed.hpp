#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_REMOVED_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_REMOVED_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/flow_entry_adaptor.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class flow_removed
        : public v13_detail::basic_openflow_message<flow_removed>
        , public v13_detail::flow_entry_adaptor<
                flow_removed, v13_detail::ofp_flow_removed
          >
    {
        static constexpr std::size_t base_flow_removed_size
            = sizeof(v13_detail::ofp_flow_removed)
            + sizeof(v13_detail::ofp_match);

    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FLOW_REMOVED;

        flow_removed(
                  oxm_match match
                , std::uint16_t const priority
                , std::uint64_t const cookie
                , protocol::ofp_flow_removed_reason const reason
                , std::uint8_t const table_id
                , v13::elapsed_time const& elapsed_time
                , v13::timeouts const& timeouts
                , v13::counters const& counters
                , std::uint32_t const xid = get_xid())
            : flow_removed_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(
                            sizeof(v13_detail::ofp_flow_removed)
                          + v13_detail::exact_length(match.length()))
                    , xid
                  }
                , cookie
                , priority
                , std::uint8_t(reason)
                , table_id
                , elapsed_time.duration_sec()
                , elapsed_time.duration_nsec()
                , timeouts.idle_timeout()
                , timeouts.hard_timeout()
                , counters.packet_count()
                , counters.byte_count()
              }
            , match_(std::move(match))
        {
        }

        flow_removed(
                  flow_entry entry
                , protocol::ofp_flow_removed_reason const reason
                , std::uint8_t const table_id
                , v13::elapsed_time const& elapsed_time
                , v13::timeouts const& timeouts
                , v13::counters const& counters
                , std::uint32_t const xid = get_xid())
            : flow_removed{
                  std::move(entry).match(), entry.priority(), entry.cookie()
                , reason
                , table_id
                , elapsed_time
                , timeouts
                , counters
                , xid
              }
        {
        }

        flow_removed(flow_removed const&) = default;

        flow_removed(flow_removed&& other)
            : flow_removed_(other.flow_removed_)
            , match_(std::move(other).match_)
        {
            other.flow_removed_.header.length = base_flow_removed_size;
        }

        auto operator=(flow_removed const&)
            -> flow_removed& = default;

        auto operator=(flow_removed&& other)
            -> flow_removed&
        {
            auto tmp = std::move(other);
            flow_removed_ = tmp.flow_removed_;
            match_.swap(tmp.match_);
            return *this;
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
        {
            return flow_removed_.header;
        }

        auto reason() const noexcept
            -> protocol::ofp_flow_removed_reason
        {
            return protocol::ofp_flow_removed_reason(flow_removed_.reason);
        }

        auto match() const noexcept
            -> oxm_match const&
        {
            return match_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_removed_);
            return match_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_removed
        {
            auto const fremoved
                = detail::decode<v13_detail::ofp_flow_removed>(first, last);

            auto const match_length
                = fremoved.header.length - sizeof(v13_detail::ofp_flow_removed);
            last = std::next(first, match_length);

            auto copy_first = first;
            auto const ofp_match
                = detail::decode<v13_detail::ofp_match>(copy_first, last);
            oxm_match::validate(ofp_match);
            if (v13_detail::exact_length(ofp_match.length) != match_length) {
                throw std::runtime_error{"invalid oxm_match length"};
            }
            auto match = oxm_match::decode(first, last);

            return flow_removed{fremoved, std::move(match)};
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < base_flow_removed_size) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        flow_removed(v13_detail::ofp_flow_removed const& fremoved
                   , oxm_match&& match)
            : flow_removed_(fremoved)
            , match_(std::move(match))
        {
        }

        friend flow_entry_adaptor;

        auto ofp_flow_entry() const noexcept
            -> v13_detail::ofp_flow_removed const&
        {
            return flow_removed_;
        }

    private:
        v13_detail::ofp_flow_removed flow_removed_;
        oxm_match match_;
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_REMOVED_HPP
