#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP

#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/net/ofp/v10/detail/byteorder.hpp>
#include <canard/net/ofp/v10/messages.hpp>
#include <canard/net/ofp/v10/openflow.hpp>
#include <canard/network/protocol/openflow/secure_channel_reader.hpp>

namespace canard {
namespace net {
namespace ofp {
namespace controller {
namespace v10 {

    struct handle_message
    {
        using header_type = net::ofp::v10::protocol::ofp_header;

        template <class Reader, class BaseChannel>
        void operator()(
                Reader* const reader, BaseChannel const& base_channel
              , header_type const& header
              , unsigned char const* first
              , unsigned char const* const last) const
        {
            if (header.version != net::ofp::v10::protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            switch (header.type) {
#           define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, net::ofp::v10::default_switch_message_list>::type; \
            case msg ## N::type(): \
                if (!msg ## N::is_valid_message_length(header)) { \
                    throw std::runtime_error{"invalid message length"}; \
                } \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                    std::tuple_size<net::ofp::v10::default_switch_message_list>::value == 10
                  , "not match to the number of message types");
            BOOST_PP_REPEAT(10, CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE
            case net::ofp::v10::protocol::OFPT_STATS_REPLY:
                if (header.length < sizeof(net::ofp::v10::protocol::ofp_stats_reply)) {
                    throw std::runtime_error{"invalid message length"};
                }
                handle_stats_reply(reader, base_channel, first, last);
                break;
            default:
                break;
            }
        }

        template <class Reader, class BaseChannel>
        void handle_stats_reply(
                Reader* const reader, BaseChannel const& base_channel
              , unsigned char const* first
              , unsigned char const* const last) const
        {
            auto const stats_reply = secure_channel_detail::read<
                net::ofp::v10::protocol::ofp_stats_reply
            >(first);
            switch (stats_reply.type) {
#           define CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, net::ofp::v10::default_stats_reply_list>::type; \
            case msg ## N::stats_type(): \
                if (!msg ## N::is_valid_stats_length(stats_reply)) { \
                    throw std::runtime_error{"invalid stats length"}; \
                } \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                    std::tuple_size<net::ofp::v10::default_stats_reply_list>::value == 6
                  , "not match to the number of stats reply types");
            BOOST_PP_REPEAT(
                    6, CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE
            default:
                break;
            }
        }
    };

    template <class ControllerHandler, class Socket>
    using secure_channel
        = secure_channel_reader<handle_message, ControllerHandler, Socket>;

    struct version
    {
        static constexpr std::uint8_t value = net::ofp::v10::protocol::OFP_VERSION;

        template <class ControllerHandler, class Socket>
        using channel_t = secure_channel<ControllerHandler, Socket>;
    };

} // namespace v10
} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
