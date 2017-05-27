#ifndef CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP
#define CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP

#include <cstddef>
#include <algorithm>
#include <utility>
#include <canard/asio/shared_buffer.hpp>

namespace canard {
namespace net {
namespace ofp {
namespace controller {

  namespace shared_buffer_generator_detail {

    struct shared_buffer_generator
    {
      using iterator = unsigned char*;
      using const_iterator = unsigned char const*;

      shared_buffer_generator() noexcept
        : buffer{}, it{nullptr}
      {
      }

      explicit shared_buffer_generator(canard::shared_buffer& buffer)
        : buffer(buffer), it{buffer.data()}
      {
      }

      explicit shared_buffer_generator(canard::shared_buffer&& buffer)
        : buffer(std::move(buffer)), it{buffer.data()}
      {
      }

      auto begin() noexcept
        -> iterator
      {
        return buffer.data();
      }

      auto begin() const noexcept
        -> const_iterator
      {
        return buffer.data();
      }

      auto end() noexcept
        -> iterator
      {
        return it;
      }

      auto end() const noexcept
        -> const_iterator
      {
        return it;
      }

      void clear() noexcept
      {
      }

      template <class Iterator>
      void insert(unsigned char*, Iterator first, Iterator last)
      {
        it = std::copy(first, last, it);
      }

      void reserve(std::size_t const size)
      {
        buffer.resize(size);
        it = buffer.data();
      }

      canard::shared_buffer buffer;
      unsigned char* it;
    };

    auto to_const_buffers(shared_buffer_generator const& buffer)
      -> canard::shared_buffer const&
    {
      return buffer.buffer;
    }

    auto to_const_buffers(shared_buffer_generator&& buffer)
      -> canard::shared_buffer&&
    {
      return std::move(buffer).buffer;
    }

  } // namespace shared_buffer_generator_detail

  using shared_buffer_generator_detail::shared_buffer_generator;

} // namespace controller
} // namespace ofp
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP
