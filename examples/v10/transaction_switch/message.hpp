#ifndef CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP
#define CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP

#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <canard/net/ofp/v10/message/error.hpp>

namespace canard {
namespace allium {
namespace decorators {

  class message_base
  {
  public:
    using error_type = canard::net::ofp::v10::messages::error;

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

  template <class Reply>
  class message
    : public message_base
  {
  public:
    using reply_type = Reply;

    message() noexcept
      : message_base{}
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
    }

    ~message() noexcept
    {
      reset();
    }

    message(message const& other)
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
      copy(other);
    }

    message(message&& other)
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
      move(std::move(other));
    }

    auto operator=(message const& other)
      -> message&
    {
      if (kind_ == other.kind_) {
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
      if (kind_ == other.kind_) {
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
      return *static_cast<reply_type const*>(this->message_base::storage_ptr_);
    }

    auto reply() noexcept
      -> reply_type&
    {
      return *static_cast<reply_type*>(this->message_base::storage_ptr_);
    }

    void reply(reply_type const& reply)
    {
      new (this->message_base::storage_ptr_) reply_type(reply);
      kind_ = msg_kind::reply;
    }

    void reply(reply_type&& reply)
    {
      new (this->message_base::storage_ptr_) reply_type(std::move(reply));
      kind_ = msg_kind::reply;
    }

  private:
    void reset() noexcept
    {
      if (kind_ == msg_kind::reply) {
        reply().~reply_type();
      }
      else if (kind_ == msg_kind::error) {
        error().~error_type();
      }
      kind_ = msg_kind::empty;
    }

    void copy(message const& other)
    {
      if (other.kind_ == msg_kind::reply) {
        reply(other.reply());
      }
      else if (other.kind_ == msg_kind::error) {
        error(other.error());
      }
    }

    void move(message&& other)
    {
      if (other.kind_ == msg_kind::reply) {
        reply(std::move(other.reply()));
      }
      else if (other.kind_ == msg_kind::error) {
        error(std::move(other.error()));
      }
    }

    void assign(message const& other)
    {
      if (kind_ == msg_kind::reply) {
        reply() = other.reply();
      }
      else if (kind_ == msg_kind::error) {
        error() = other.error();
      }
    }

    void assign(message&& other)
    {
      if (kind_ == msg_kind::reply) {
        reply() = std::move(other.reply());
      }
      else if (kind_ == msg_kind::error) {
        error() = std::move(other.error());
      }
    }

    typename std::aligned_union<1, reply_type, error_type>::type storage_;
  };

  template <>
  class message<void>
    : public message_base
  {
  public:
    message() noexcept
      : message_base{}
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
    }

    message(message const& other)
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
      if (other.kind_ == msg_kind::error) {
        error(other.error());
      }
    }

    message(message&& other)
    {
      this->message_base::storage_ptr_ = std::addressof(storage_);
      if (other.kind_ == msg_kind::error) {
        error(std::move(other.error()));
      }
    }

    auto operator=(message const& other)
      -> message
    {
      if (kind_ == other.kind_) {
        if (kind_ == msg_kind::error) {
          error() = other.error();
        }
      }
      else {
        reset();
        if (other.kind_ == msg_kind::error) {
          error(other.error());
        }
      }
      return *this;
    }

    auto operator=(message&& other)
      -> message
    {
      if (kind_ == other.kind_) {
        if (kind_ == msg_kind::error) {
          error() = std::move(other.error());
        }
      }
      else {
        reset();
        if (other.kind_ == msg_kind::error) {
          error(std::move(other.error()));
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
      return error();
    }

    auto operator*() const noexcept
      -> error_type const&
    {
      return error();
    }

    auto operator->() noexcept
      -> error_type*
    {
      return std::addressof(error());
    }

    auto operator->() const noexcept
      -> error_type const*
    {
      return std::addressof(error());
    }

  private:
    void reset() noexcept
    {
      if (kind_ == msg_kind::error) {
        error().~error_type();
      }
      kind_ = msg_kind::empty;
    }

    std::aligned_union<1, error_type> storage_;
  };

} // namespace decorators
} // namespace allium
} // namespace canard

#endif // CANARD_ALLIUM_DECORATORS_TRANSACTION_MESSAGE_HPP
