#ifndef CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP
#define CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP

#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>

namespace canard {
namespace allium {
namespace decorators {

  template <class Error>
  class message_base
  {
  public:
    using error_type = Error;

    explicit operator bool() const noexcept
    {
      return kind_ != msg_kind::empty;
    }

    auto is_reply() const noexcept
      -> bool
    {
      return kind_ == msg_kind::reply;
    }

    auto is_error() const noexcept
      -> bool
    {
      return kind_ == msg_kind::error;
    }

    auto error() const noexcept
      -> error_type const&
    {
      return *static_cast<error_type const*>(storage_ptr_);
    }

    auto error() noexcept
      -> error_type&
    {
      return *static_cast<error_type*>(storage_ptr_);
    }

    void error(error_type const& error)
    {
      new (storage_ptr_) error_type(error);
      kind_ = msg_kind::error;
    }

    void error(error_type&& error)
    {
      new (storage_ptr_) error_type(std::move(error));
      kind_ = msg_kind::error;
    }

  protected:
    enum class msg_kind : std::uint8_t { empty, error, reply };

    explicit message_base() noexcept
      : storage_ptr_(nullptr)
      , kind_(msg_kind::empty)
    {
    }

    void* storage_ptr_;
    msg_kind kind_;
  };

  template <class Reply, class Error>
  class message
    : public message_base<Error>
  {
    using base_t = message_base<Error>;
    using error_type = Error;

  public:
    using reply_type = Reply;

    message() noexcept
      : base_t{}
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
    }

    ~message() noexcept
    {
      reset();
    }

    message(message const& other)
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
      copy(other);
    }

    message(message&& other)
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
      move(std::move(other));
    }

    auto operator=(message const& other)
      -> message&
    {
      if (this->kind_ == other.kind_) {
        assign(other);
      }
      else {
        reset();
        copy(other);
      }
      return *this;
    }

    auto operator=(message&& other)
      -> message
    {
      if (this->kind_ == other.kind_) {
        assign(std::move(other));
      }
      else {
        reset();
        copy(other);
      }
      return *this;
    }

    auto reply() const noexcept
      -> reply_type const&
    {
      return *static_cast<reply_type const*>(this->base_t::storage_ptr_);
    }

    auto reply() noexcept
      -> reply_type&
    {
      return *static_cast<reply_type*>(this->base_t::storage_ptr_);
    }

    void reply(reply_type const& reply)
    {
      new (this->base_t::storage_ptr_) reply_type(reply);
      this->kind_ = base_t::msg_kind::reply;
    }

    void reply(reply_type&& reply)
    {
      new (this->base_t::storage_ptr_) reply_type(std::move(reply));
      this->kind_ = base_t::msg_kind::reply;
    }

  private:
    void reset() noexcept
    {
      if (this->kind_ == base_t::msg_kind::reply) {
        reply().~reply_type();
      }
      else if (this->kind_ == base_t::msg_kind::error) {
        this->error().~error_type();
      }
      this->kind_ = base_t::msg_kind::empty;
    }

    void copy(message const& other)
    {
      if (other.kind_ == base_t::msg_kind::reply) {
        reply(other.reply());
      }
      else if (other.kind_ == base_t::msg_kind::error) {
        this->error(other.error());
      }
    }

    void move(message&& other)
    {
      if (other.kind_ == base_t::msg_kind::reply) {
        reply(std::move(other.reply()));
      }
      else if (other.kind_ == base_t::msg_kind::error) {
        this->error(std::move(other.error()));
      }
    }

    void assign(message const& other)
    {
      if (this->kind_ == base_t::msg_kind::reply) {
        reply() = other.reply();
      }
      else if (this->kind_ == base_t::msg_kind::error) {
        this->error() = other.error();
      }
    }

    void assign(message&& other)
    {
      if (this->kind_ == base_t::msg_kind::reply) {
        reply() = std::move(other.reply());
      }
      else if (this->kind_ == base_t::msg_kind::error) {
        this->error() = std::move(other.error());
      }
    }

    typename std::aligned_union<1, reply_type, error_type>::type storage_;
  };

  template <class Error>
  class message<void, Error>
    : public message_base<Error>
  {
    using base_t = message_base<Error>;
    using error_type = Error;

  public:
    message() noexcept
      : base_t{}
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
    }

    message(message const& other)
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
      if (other.kind_ == base_t::msg_kind::error) {
        this->error(other.error());
      }
    }

    message(message&& other)
    {
      this->base_t::storage_ptr_ = std::addressof(storage_);
      if (other.kind_ == base_t::msg_kind::error) {
        this->error(std::move(other.error()));
      }
    }

    auto operator=(message const& other)
      -> message
    {
      if (this->kind_ == other.kind_) {
        if (this->kind_ == base_t::msg_kind::error) {
          this->error() = other.error();
        }
      }
      else {
        reset();
        if (other.kind_ == base_t::msg_kind::error) {
          this->error(other.error());
        }
      }
      return *this;
    }

    auto operator=(message&& other)
      -> message
    {
      if (this->kind_ == other.kind_) {
        if (this->kind_ == base_t::msg_kind::error) {
          this->error() = std::move(other.error());
        }
      }
      else {
        reset();
        if (other.kind_ == base_t::msg_kind::error) {
          this->error(std::move(other.error()));
        }
      }
      return *this;
    }

    ~message() noexcept
    {
      reset();
    }

    auto operator*() noexcept
      -> error_type&
    {
      return this->error();
    }

    auto operator*() const noexcept
      -> error_type const&
    {
      return this->error();
    }

    auto operator->() noexcept
      -> error_type*
    {
      return std::addressof(this->error());
    }

    auto operator->() const noexcept
      -> error_type const*
    {
      return std::addressof(this->error());
    }

  private:
    void reset() noexcept
    {
      if (this->kind_ == base_t::msg_kind::error) {
        this->error().~error_type();
      }
      this->kind_ = base_t::msg_kind::empty;
    }

    std::aligned_union<1, error_type> storage_;
  };

} // namespace decorators
} // namespace allium
} // namespace canard

#endif // CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP
