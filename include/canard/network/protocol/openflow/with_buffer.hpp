#ifndef CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP
#define CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <utility>

namespace canard {
namespace network {
namespace openflow {

    template <class Buffer>
    void openflow_buffer_reserve(Buffer& buffer, std::size_t const size)
    {
        buffer.reserve(size);
    }

    template <class Buffer, class Range>
    auto openflow_buffer_push_back(Buffer& buffer, Range&& range)
        -> Buffer&
    {
        buffer.push_back(std::forward<Range>(range));
        return buffer;
    }

    template <class Buffer>
    auto openflow_buffer_to_const_buffers(Buffer&& buffer)
        -> decltype(std::forward<Buffer>(buffer).to_const_buffers())
    {
        return std::forward<Buffer>(buffer).to_const_buffers();
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
                -> decltype(openflow_buffer_to_const_buffers(
                            std::declval<Buffer>()))
            {
                openflow_buffer_reserve(buffer, message.length());
                message.encode(buffer);
                return openflow_buffer_to_const_buffers(
                        std::forward<Buffer>(buffer));
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

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_WITH_BUFFER_HPP
