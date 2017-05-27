#ifndef CANARD_ASIO_SHARED_BUFFER_HPP
#define CANARD_ASIO_SHARED_BUFFER_HPP

#include <cstddef>
#include <atomic>
#include <new>
#include <boost/asio/buffer.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/intrusive_ptr.hpp>

namespace canard {

  namespace detail {

    struct atomic_counter
    {
      std::atomic<std::size_t> counter_;

      constexpr atomic_counter(std::size_t const count) noexcept
        : counter_{count}
      {
      }

      auto operator++() noexcept
        -> std::size_t
      {
        return counter_.fetch_add(1, std::memory_order_relaxed) + 1;
      }

      auto operator--() noexcept
        -> std::size_t
      {
        auto const counter
          = counter_.fetch_sub(1, std::memory_order_release) - 1;
        if (counter == 0) {
          std::atomic_thread_fence(std::memory_order_acquire);
        }
        return counter;
      }
    };

    template <class Counter>
    class ref_count_buffer
    {
    public:
      ref_count_buffer(void* const ptr, std::size_t const size)
        : ref_count_{1}
        , buffer_{ptr, size}
      {
      }

      auto buffer() const noexcept
        -> boost::asio::mutable_buffer const&
      {
        return buffer_;
      }

      auto size() const
        -> std::size_t
      {
        return boost::asio::buffer_size(buffer_);
      }

      auto data() const
        -> unsigned char*
      {
        return boost::asio::buffer_cast<unsigned char*>(buffer_);
      }

      friend void intrusive_ptr_add_ref(ref_count_buffer* const p) noexcept
      {
        ++p->ref_count_;
      }

      friend void intrusive_ptr_release(ref_count_buffer* const p) noexcept
      {
        if (--p->ref_count_ == 0) {
          auto const pointer = static_cast<void*>(p);
          p->~ref_count_buffer();
          operator delete(pointer);
        }
      }

    private:
      Counter ref_count_;
      boost::asio::mutable_buffer buffer_;
    };

    template <class Counter>
    static auto make_ref_count_buffer(std::size_t const size)
      -> ref_count_buffer<Counter>*
    {
      constexpr auto obj_size = sizeof(ref_count_buffer<Counter>);
      auto const pointer = operator new(obj_size + size);
      return new(pointer) ref_count_buffer<Counter>{
        static_cast<unsigned char*>(pointer) + obj_size, size
      };
    }

  } // namespace detail

  template <class Counter = detail::atomic_counter>
  class basic_shared_buffer
  {
  public:
    using value_type = boost::asio::mutable_buffer;

    struct const_iterator
      : boost::iterator_facade<
            const_iterator
          , value_type
          , boost::random_access_traversal_tag
          , value_type const&
        >
    {
      using base_type = boost::iterator_facade<
          const_iterator
        , value_type
        , boost::random_access_traversal_tag
        , value_type const&
      >;

      const_iterator() noexcept
        : ptr()
      {}

      explicit const_iterator(
          detail::ref_count_buffer<Counter> const* const ptr) noexcept
        : ptr(ptr)
      {}

      auto dereference() const noexcept
        -> typename base_type::reference
      { return ptr->buffer(); }

      auto equal(const_iterator other) const noexcept
        -> bool
      { return ptr == other.ptr; }

      void increment() noexcept
      { ++ptr; }

      void decrement() noexcept
      { --ptr; }

      void advance(typename base_type::difference_type const n) noexcept
      { ptr += n; }

      auto distance_to(const_iterator other) const
        -> typename base_type::difference_type
      { return std::distance(ptr, other.ptr); }

      detail::ref_count_buffer<Counter> const* ptr;
    };

    basic_shared_buffer() noexcept
      : data_{}
    {
    }

    explicit basic_shared_buffer(std::size_t const buffer_size)
      : data_{detail::make_ref_count_buffer<Counter>(buffer_size), false}
    {
    }

    auto begin() const noexcept
      -> const_iterator
    {
      return const_iterator{data_.get()};
    }

    auto end() const noexcept
      -> const_iterator
    {
      return const_iterator{data_.get() + 1};
    }

    auto data() const
      -> unsigned char*
    {
      return data_->data();
    }

    auto size() const
      -> std::size_t
    {
      return data_->size();
    }

    void resize(std::size_t const buffer_size)
    {
      data_.reset(detail::make_ref_count_buffer<Counter>(buffer_size), false);
    }

  private:
    boost::intrusive_ptr<detail::ref_count_buffer<Counter>> data_;
  };

  using shared_buffer = basic_shared_buffer<>;
  using thread_unsafe_shared_buffer = basic_shared_buffer<std::size_t>;

} // namespace canard

#endif // CANARD_ASIO_SHARED_BUFFER_HPP
