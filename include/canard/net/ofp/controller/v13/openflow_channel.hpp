#ifndef CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP

#include <cstdint>
#include <tuple>
#include <boost/preprocessor/repeat.hpp>
#include <canard/net/ofp/controller/secure_channel_reader.hpp>
#include <canard/net/ofp/v13/detail/byteorder.hpp>
#include <canard/net/ofp/v13/messages.hpp>
#include <canard/net/ofp/v13/openflow.hpp>

namespace canard {
namespace net {
namespace ofp {
namespace controller {
namespace v13 {

  struct handle_message
  {
    using header_type = net::ofp::v13::protocol::ofp_header;

    template <class Reader, class BaseChannel>
    void operator()(
          Reader* const reader, BaseChannel const& base_channel
        , header_type const& header
        , unsigned char const* first
        , unsigned char const* const last) const
    {
      using message_list = net::ofp::v13::default_switch_message_list;
      switch (header.type) {

#     define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE(z, N, _) \
      using msg ## N = std::tuple_element<N, message_list>::type; \
      case msg ## N::message_type: \
        reader->handle(base_channel, msg ## N::decode(first, last)); \
        break;
      static_assert(
            std::tuple_size<message_list>::value == 12
          , "not match to the number of message types");
      BOOST_PP_REPEAT(10, CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE, _)
#     undef  CANARD_NETWORK_OPENFLOW_V13_MESSAGES_CASE

      case net::ofp::v13::protocol::OFPT_MULTIPART_REPLY:
        if (header.length < sizeof(net::ofp::v13::protocol::ofp_multipart_reply)) {
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
        net::ofp::v13::protocol::ofp_multipart_reply
      >(first);
      using multipart_reply_list = net::ofp::v13::default_multipart_reply_list;
      switch (multipart_reply.type) {

#     define CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE(z, N, _) \
      using msg ## N = std::tuple_element<N, multipart_reply_list>::type; \
      case msg ## N::multipart_type_value: \
        reader->handle(base_channel, msg ## N::decode(first, last)); \
        break;
      static_assert(
            std::tuple_size<multipart_reply_list>::value == 14
          , "not match to the number of multipart reply types");
      BOOST_PP_REPEAT(14, CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE, _)
#     undef  CANARD_NETWORK_OPENFLOW_V13_MULTIPART_REPLY_CASE

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
    static constexpr std::uint8_t value = net::ofp::v13::protocol::OFP_VERSION;

    template <class ControllerHandler, class Socket>
    using channel_t = openflow_channel<ControllerHandler, Socket>;
  };

} // namespace v13
} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_CHANNLE_HPP
