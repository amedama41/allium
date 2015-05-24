#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BASIC_STATS_MESSAGE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BASIC_STATS_MESSAGE_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace messages_detail {

    template <class T, class DecodeReturnType = T>
    class basic_stats_request
        : public v10_detail::basic_openflow_message<T>
    {
    public:
        static ofp_type const message_type = OFPT_STATS_REQUEST;

    protected:
        explicit basic_stats_request(
                std::uint32_t const xid, std::uint16_t const body_length = 0)
            : stats_request_{
                 {OFP_VERSION, message_type, std::uint16_t(sizeof(stats_request_) + body_length), xid}
                , T::stats_type_value
                , 0
              }
        {
        }

    public:
        auto header() const
            -> v10_detail::ofp_header
        {
            return stats_request_.header;
        }

        auto stats_type() const
            -> ofp_stats_types
        {
            return T::stats_type_value;
        }

        auto flags() const
            -> std::uint16_t
        {
            return stats_request_.flags;
        }

        auto more() const
            -> bool
        {
            return flags() & OFPSF_REPLY_MORE;
        }

    protected:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, stats_request_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> DecodeReturnType
        {
            return DecodeReturnType(detail::decode<v10_detail::ofp_stats_request>(first, last));
        }

    protected:
        basic_stats_request(
                  v10_detail::ofp_stats_request const& stats_request
                , std::uint16_t const body_length)
            : stats_request_(stats_request)
        {
            if (stats_request_.header.version != v10::OFP_VERSION) {
                throw 1;
            }
            if (stats_request_.header.type != message_type) {
                throw 2;
            }
            if (stats_request_.header.length != sizeof(stats_request_) + body_length) {
                throw 3;
            }
            if (stats_request_.type != T::stats_type_value) {
                throw 4;
            }
        }

    private:
        v10_detail::ofp_stats_request stats_request_;
    };


    template <class T, class DecodeReturnType>
    class basic_stats_reply
        : public v10_detail::basic_openflow_message<T>
    {
    public:
        static ofp_type const message_type = OFPT_STATS_REPLY;

    protected:
        basic_stats_reply(
                  std::uint32_t const xid
                , std::uint16_t const body_length, std::uint16_t const flags)
            : stats_reply_{
                  {OFP_VERSION, message_type, sizeof(stats_reply_) + body_length, xid}
                , T::stats_type_value
                , flags
              }
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return stats_reply_.header;
        }

        auto stats_type() const
            -> ofp_stats_types
        {
            return T::stats_type_value;
        }

        auto flags() const
            -> std::uint16_t
        {
            return stats_reply_.flags;
        }

        auto more() const
            -> bool
        {
            return flags() & OFPSF_REPLY_MORE;
        }

        void more(bool const has_more)
        {
            if (has_more) {
                stats_reply_.flags |= OFPSF_REPLY_MORE;
            }
            else {
                stats_reply_.flags &= ~OFPSF_REPLY_MORE;
            }
        }

    protected:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, stats_reply_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> DecodeReturnType
        {
            return DecodeReturnType(detail::decode<v10_detail::ofp_stats_reply>(first, last));
        }

    protected:
        basic_stats_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , std::uint16_t const body_length)
            : stats_reply_(stats_reply)
        {
            if (stats_reply_.header.version != v10::OFP_VERSION) {
                throw 1;
            }
            if (stats_reply_.header.type != message_type) {
                throw 2;
            }
            if (stats_reply_.header.length != sizeof(stats_reply_) + body_length) {
                throw 3;
            }
            if (stats_reply_.type != T::stats_type_value) {
                throw 4;
            }
        }

    private:
        v10_detail::ofp_stats_reply stats_reply_;
    };

} // namespace messages_detail
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BASIC_STATS_MESSAGE_HPP
