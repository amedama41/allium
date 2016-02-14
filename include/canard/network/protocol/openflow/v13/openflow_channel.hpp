#ifndef CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP

#include <cstdint>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/network/protocol/openflow/secure_channel_reader.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    struct handle_message
    {
        using header_type = v13_detail::ofp_header;

        template <class Reader, class BaseChannel>
        void operator()(
                  Reader* const reader, BaseChannel const& base_channel
                , header_type const& header
                , unsigned char const* first
                , unsigned char const* const last) const
        {
            switch (header.type) {
#           define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, default_switch_message_list>::type; \
            case msg ## N::message_type: \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                      std::tuple_size<default_switch_message_list>::value == 10
                    , "not match to the number of message types");
            BOOST_PP_REPEAT(10, CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE
            case protocol::OFPT_MULTIPART_REPLY:
                if (header.length < sizeof(v13_detail::ofp_multipart_reply)) {
                    // TODO needs error handling
                    break;
                }
                handle_multipart_reply(reader, base_channel, first, last);
                break;
            default:
                break;
            }
        }

        template <class Reader, class BaseChannel>
        void handle_multipart_reply(
                  Reader* const reader, BaseChannel const& base_channel
                , unsigned char const* first
                , unsigned char const* const last) const
        {
            auto const multipart_reply = secure_channel_detail::read<
                v13_detail::ofp_multipart_reply
            >(first);
            switch (multipart_reply.type) {
#           define CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE(z, N, _) \
            using msg ## N \
                = std::tuple_element<N, default_multipart_reply_list>::type; \
            case msg ## N::multipart_type_value: \
                reader->handle(base_channel, msg ## N::decode(first, last)); \
                break;
            static_assert(
                      std::tuple_size<default_multipart_reply_list>::value == 8
                    , "not match to the number of multipart reply types");
            BOOST_PP_REPEAT(
                    8, CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE
            default:
                break;
            }
        }
    };

    template <class ControllerHandler, class Socket>
    using openflow_channel
        = secure_channel_reader<handle_message, ControllerHandler, Socket>;

    struct version
    {
        static constexpr std::uint8_t value = v13::protocol::OFP_VERSION;

        template <class ControllerHandler, class Socket>
        using channel_t = openflow_channel<ControllerHandler, Socket>;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
