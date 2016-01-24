#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP

#include <cstdint>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/secure_channel_reader.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/messages.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    struct handle_message
    {
        using header_type = v10_detail::ofp_header;

        template <class Reader, class BaseChannel>
        void operator()(
                Reader* const reader, BaseChannel const& base_channel
              , header_type const& header
              , unsigned char const* first
              , unsigned char const* const last) const
        {
            switch (header.type) {
#           define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, default_switch_message_list>::type; \
            case msg ## N::message_type: \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                    std::tuple_size<default_switch_message_list>::value == 10
                  , "not match to the number of message types");
            BOOST_PP_REPEAT(10, CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V10_MESSAGES_CASE
            case protocol::OFPT_STATS_REPLY:
                if (header.length < sizeof(v10_detail::ofp_stats_reply)) {
                    // TODO needs error handling
                    break;
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
                v10_detail::ofp_stats_reply
            >(first);
            switch (stats_reply.type) {
#           define CANARD_NETWORK_OPENFLOW_V10_STATS_REPLY_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, default_stats_reply_list>::type; \
            case msg ## N::stats_type_value: \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                    std::tuple_size<default_stats_reply_list>::value == 6
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
        static constexpr std::uint8_t value = v10::protocol::OFP_VERSION;

        template <class ControllerHandler, class Socket>
        using channel_t = secure_channel<ControllerHandler, Socket>;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
