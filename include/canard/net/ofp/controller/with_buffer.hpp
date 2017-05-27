#ifndef CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP
#define CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <boost/asio/buffer.hpp>

namespace canard {
namespace net {
namespace ofp {
namespace controller {

  template <class Buffer>
  auto to_const_buffers(Buffer const& buffer)
    -> boost::asio::const_buffers_1
  {
    return boost::asio::buffer(buffer);
  }

  namespace detail {

    template <class Message, class Buffer>
    struct message_with_buffer
    {
      auto header() const noexcept
        -> decltype(std::declval<Message>().header())
      {
        return message.header();
      }

      auto version() const noexcept
        -> std::uint8_t
      {
        return message.version();
      }

      auto type() const noexcept
        -> std::uint8_t
      {
        return message.type();
      }

      auto length() const noexcept
        -> std::uint16_t
      {
        return message.length();
      }

      auto xid() const noexcept
        -> std::uint32_t
      {
        return message.xid();
      }

      auto encode() const
        -> decltype(to_const_buffers(std::declval<Buffer>()))
      {
        buffer.clear();
        buffer.reserve(message.length());
        message.encode(buffer);
        return to_const_buffers(std::forward<Buffer>(buffer));
      }

      Message message;
      Buffer buffer;
    };

  } // namespace detail

  template <class Message, class Buffer>
  auto with_buffer(Message const& msg, Buffer&& buffer)
    -> detail::message_with_buffer<Message const&, Buffer&&>
  {
    return detail::message_with_buffer<Message const&, Buffer&&>{
      msg, std::forward<Buffer>(buffer)
    };
  }

} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP
