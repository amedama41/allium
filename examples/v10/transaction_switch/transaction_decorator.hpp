#ifndef CANARD_ALLIUM_DECORATORS_TRANSACTION_TRANSACTION_DECORATOR_HPP
#define CANARD_ALLIUM_DECORATORS_TRANSACTION_TRANSACTION_DECORATOR_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/asio_handler_hook_propagation.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/net/ofp/controller/v10/secure_channel.hpp>
#include "./message.hpp"

namespace ofp = canard::net::ofp;

namespace canard {
namespace allium {
namespace decorators {

  template <class Base>
  class transaction_decorator;

  enum class message_kind { request, reply, other };

  template <class Message>
  struct message_traits
  {
    static constexpr message_kind kind = message_kind::other;
  };

  template <>
  struct message_traits<ofp::v10::messages::echo_request>
  {
    static constexpr message_kind kind = message_kind::request;
    using reply_type = ofp::v10::messages::echo_reply;
  };
  template <>
  struct message_traits<ofp::v10::messages::features_request>
  {
    static constexpr message_kind kind = message_kind::request;
    using reply_type = ofp::v10::messages::features_reply;
  };
  template <>
  struct message_traits<ofp::v10::messages::get_config_request>
  {
    static constexpr message_kind kind = message_kind::request;
    using reply_type = ofp::v10::messages::get_config_reply;
  };
  template <>
  struct message_traits<ofp::v10::messages::barrier_request>
  {
    static constexpr message_kind kind = message_kind::request;
    using reply_type = ofp::v10::messages::barrier_reply;
  };
  template <>
  struct message_traits<ofp::v10::messages::queue_get_config_request>
  {
    static constexpr message_kind kind = message_kind::request;
    using reply_type = ofp::v10::messages::queue_get_config_reply;
  };
  template <>
  struct message_traits<ofp::v10::messages::echo_reply>
  {
    static constexpr message_kind kind = message_kind::reply;
    using request_type = ofp::v10::messages::echo_request;
  };
  template <>
  struct message_traits<ofp::v10::messages::features_reply>
  {
    static constexpr message_kind kind = message_kind::reply;
    using request_type = ofp::v10::messages::features_request;
  };
  template <>
  struct message_traits<ofp::v10::messages::get_config_reply>
  {
    static constexpr message_kind kind = message_kind::reply;
    using request_type = ofp::v10::messages::get_config_request;
  };
  template <>
  struct message_traits<ofp::v10::messages::barrier_reply>
  {
    static constexpr message_kind kind = message_kind::reply;
    using request_type = ofp::v10::messages::barrier_request;
  };
  template <>
  struct message_traits<ofp::v10::messages::queue_get_config_reply>
  {
    static constexpr message_kind kind = message_kind::reply;
    using request_type = ofp::v10::messages::queue_get_config_request;
  };


  namespace transaction_decorator_detail {

    template <class T>
    using decay_t = typename std::decay<T>::type;

    template <class T>
    auto has_type(T const&) -> decltype(T::type(), std::true_type{});
    auto has_type(...) -> std::false_type;

    template <class T, class U = void>
    using enable_if_has_type_t = typename std::enable_if<
        decltype(has_type(std::declval<decay_t<T>>()))::value
      , U
    >::type;

    struct is_reply {};
    struct is_error {};
    struct is_other {};

    template <class T>
    using message_kind_t = typename std::conditional<
        message_traits<T>::kind == message_kind::reply
      , is_reply
      , typename std::conditional<
          T::type() == ofp::v10::protocol::OFPT_ERROR, is_error, is_other
        >::type
    >::type;


    constexpr auto to_key(
          std::uint16_t const msg_type
        , std::uint16_t const stats_type
        , std::uint32_t const xid) noexcept
      -> std::uint64_t
    {
      return std::uint64_t{msg_type} << 48
           | std::uint64_t{stats_type} << 32
           | std::uint64_t{xid};
    }

    template <
        class T
      , bool = (T::type() == ofp::v10::protocol::OFPT_STATS_REQUEST
             || T::type() == ofp::v10::protocol::OFPT_STATS_REPLY)
    >
    struct stats_type_if_any
    {
      static constexpr std::uint16_t value = 0;
    };

    template <class T>
    struct stats_type_if_any<T, true>
    {
      static constexpr std::uint16_t value = T::stats_type();
    };

    inline static auto get_stats_type(ofp::v10::messages::error const& error)
      -> std::uint16_t
    {
      ofp::v10::protocol::ofp_stats_request stats_request;
      std::memcpy(&stats_request, error.data().data(), sizeof(stats_request));
      boost::endian::big_to_native_inplace(stats_request);
      return stats_request.type;
    }


    template <class SendRequestHandler, class Transaction>
    struct send_handler_adaptor
      : public canard::asio_handler_hook_propagation<
          send_handler_adaptor<SendRequestHandler, Transaction>
        >
    {
      template <class Handler>
      send_handler_adaptor(Handler&& h, std::shared_ptr<Transaction>&& txn)
        : handler_(std::forward<Handler>(h))
        , txn_(std::move(txn))
      {
      }

      void operator()(boost::system::error_code const& ec, std::size_t)
      {
        if (!ec) {
          handler_(ec, std::move(txn_));
        }
        else {
          handler_(ec, std::shared_ptr<Transaction>{});
        }
      }

      auto handler() noexcept
        -> SendRequestHandler&
      {
        return handler_;
      }

      SendRequestHandler handler_;
      std::shared_ptr<Transaction> txn_;
    };

    template <class SendRequestHandler, class Transaction>
    static auto make_send_handler_adaptor(
        SendRequestHandler&& handler, std::shared_ptr<Transaction>&& txn)
      -> send_handler_adaptor<decay_t<SendRequestHandler>, Transaction>
    {
      return send_handler_adaptor<decay_t<SendRequestHandler>, Transaction>{
        std::forward<SendRequestHandler>(handler), std::move(txn)
      };
    }

    template <class ReceiveResponseHandler, class Transaction>
    struct wait_handler_adaptor
      : public canard::asio_handler_hook_propagation<
            wait_handler_adaptor<ReceiveResponseHandler, Transaction>
          , canard::no_propagation_hook_invoke
        >
    {
      template <class Handler>
      wait_handler_adaptor(Handler&& h, std::shared_ptr<Transaction> const& txn)
        : handler_(std::forward<Handler>(h))
        , txn_(txn)
      {
      }

      void operator()(boost::system::error_code const& ec)
      {
        if (txn_->msg) {
          handler_(boost::system::error_code{}, std::move(txn_->msg));
        }
        else {
          handler_(ec, typename Transaction::message_type{});
        }
      }

      auto handler() noexcept
        -> ReceiveResponseHandler&
      {
        return handler_;
      }

      ReceiveResponseHandler handler_;
      std::shared_ptr<Transaction> txn_;
    };

    template <class ReceiveResponseHandler, class Transaction>
    auto make_wait_handler_adaptor(
        ReceiveResponseHandler&& h, std::shared_ptr<Transaction> const& txn)
      -> wait_handler_adaptor<decay_t<ReceiveResponseHandler>, Transaction>
    {
      return wait_handler_adaptor<decay_t<ReceiveResponseHandler>, Transaction>{
        std::forward<ReceiveResponseHandler>(h), txn
      };
    }


    struct transaction_base
    {
      using timer_type = boost::asio::steady_timer;

      boost::asio::io_service::strand context;
      timer_type timer;
      message_base* msg_ptr;

    protected:
      explicit transaction_base(boost::asio::io_service::strand strand)
        : context(strand)
        , timer{context.get_io_service()}
        , msg_ptr(nullptr)
      {
      }
    };

    template <class T, message_kind Kind = message_traits<T>::kind>
    struct message_type
    {
      using type = message<void>;
    };

    template <class T>
    struct message_type<T, message_kind::request>
    {
      using type = message<typename message_traits<T>::reply_type>;
    };

  } // namespace transaction_decorator_detail


  template <class Reply>
  class transaction
    : public transaction_decorator_detail::transaction_base
  {
  public:
    using message_type
      = typename transaction_decorator_detail::message_type<Reply>::type;

  private:
    template <class> friend class transaction_decorator;
    template <class, class>
    friend struct transaction_decorator_detail::wait_handler_adaptor;

    explicit transaction(boost::asio::io_service::strand strand)
      : transaction_base{strand}
      , msg{}
    {
      this->transaction_base::msg_ptr = &msg;
    }

    message_type msg;
  };


  template <class Base>
  class transaction_decorator
    : public Base
  {
    using transaction_base_ptr
      = std::shared_ptr<transaction_decorator_detail::transaction_base>;
    using weak_transaction_base_ptr
      = std::weak_ptr<transaction_decorator_detail::transaction_base>;

    class transaction_data
    {
      friend transaction_decorator;
      std::unordered_map<std::uint64_t, weak_transaction_base_ptr> map;
      std::mutex mutex;
    };

  public:
    using channel_data = transaction_data;

    using clock_type
      = transaction_decorator_detail::transaction_base::timer_type::clock_type;
    template <class Request>
    using transaction_ptr = std::shared_ptr<transaction<Request>>;

  private:
    template <class Request>
    using send_request_handler_type
      = void(boost::system::error_code, transaction_ptr<Request>);
    template <class Request>
    using receive_response_handler_type
      = void(boost::system::error_code
           , typename transaction_ptr<Request>::element_type::message_type);
    template <class SendRequestHandler, class Message>
    using async_send_request_result_init = canard::async_result_init<
        typename std::decay<SendRequestHandler>::type
      , send_request_handler_type<typename std::decay<Message>::type>
    >;
    template <class ReceiveResponseHandler, class Transaction>
    using async_receive_response_result_init = canard::async_result_init<
        typename std::decay<ReceiveResponseHandler>::type
      , void(boost::system::error_code, typename Transaction::message_type)
    >;

  public:
    template <
        class Channel, class Message
      , class = transaction_decorator_detail::enable_if_has_type_t<Message>
    >
    void handle(Channel&& channel, Message&& msg)
    {
      handle_impl(
            std::forward<Channel>(channel), std::forward<Message>(msg)
          , transaction_decorator_detail::message_kind_t<Message>{});
    }

    template <class... Args>
    void handle(Args&&... args)
    {
      this->forward(std::forward<Args>(args)...);
    }

    template <class Channel, class Request, class SendRequestHandler>
    static auto async_send_request(
        Channel const& channel, Request&& request, SendRequestHandler&& handler)
      -> typename async_send_request_result_init<
           SendRequestHandler, Request
         >::result_type
    {
      async_send_request_result_init<SendRequestHandler, Request> init{
        std::forward<SendRequestHandler>(handler)
      };
      auto txn_ptr = register_request(channel, request);
      channel->async_send(
            std::forward<Request>(request)
          , transaction_decorator_detail::make_send_handler_adaptor(
                std::move(init.handler()), std::move(txn_ptr)));
      return init.get();
    }

    template <class Transaction, class ReceiveResponseHandler>
    static auto async_receive_response(
          std::shared_ptr<Transaction> const& txn
        , ReceiveResponseHandler&& handler)
      -> typename async_receive_response_result_init<
           ReceiveResponseHandler, Transaction
         >::result_type
    {
      return async_receive_response(
            txn, clock_type::duration::max()
          , std::forward<ReceiveResponseHandler>(handler));
    }

    template <class Transaction, class ReceiveResponseHandler>
    static auto async_receive_response(
          std::shared_ptr<Transaction> const& txn
        , clock_type::duration const& timeout
        , ReceiveResponseHandler&& handler)
      -> typename async_receive_response_result_init<
           ReceiveResponseHandler, Transaction
         >::result_type
    {
      async_receive_response_result_init<ReceiveResponseHandler, Transaction>
        init{std::forward<ReceiveResponseHandler>(handler)};

      auto ctx = txn->context;
      if (ctx.running_in_this_thread()) {
        async_wait(txn, timeout, std::move(init.handler()));
      }
      else {
        using handler_type = typename async_receive_response_result_init<
          ReceiveResponseHandler, Transaction
        >::handler_type;
        ctx.post(async_wait_functor<handler_type, Transaction>{
              std::move(init.handler()), txn, timeout});
      }

      return init.get();
    }

  private:
    template <class Transaction, class ReceiveResponseHandler>
    static void async_wait(
          std::shared_ptr<Transaction> const& txn
        , boost::asio::steady_timer::duration const& timeout
        , ReceiveResponseHandler&& handler)
    {
      auto ctx = txn->context;
      if (txn->msg) {
        ctx.post(canard::detail::bind(
                std::forward<ReceiveResponseHandler>(handler)
              , boost::system::error_code{}, std::move(txn->msg)));
      }
      else {
        txn->timer.expires_from_now(timeout);
        txn->timer.async_wait(
            ctx.wrap(transaction_decorator_detail::make_wait_handler_adaptor(
                std::forward<ReceiveResponseHandler>(handler), txn)));
      }
    }

    template <class ReceiveResponseHandler, class Transaction>
    struct async_wait_functor
      : canard::asio_handler_hook_propagation<
            async_wait_functor<ReceiveResponseHandler, Transaction>
          , canard::no_propagation_hook_invoke
        >
    {
      template <class Handler>
      async_wait_functor(
            Handler&& h
          , std::shared_ptr<Transaction> const& txn
          , clock_type::duration const& timeout)
        : handler_(std::forward<Handler>(h))
        , txn_(txn)
        , timeout_(timeout)
      {
      }

      void operator()()
      {
        async_wait(txn_, timeout_, std::move(handler_));
      }

      auto handler() noexcept
        -> ReceiveResponseHandler&
      {
        return handler_;
      }

      ReceiveResponseHandler handler_;
      std::shared_ptr<Transaction> txn_;
      clock_type::duration timeout_;
    };

    template <class Reply>
    static void set_reply(transaction_base_ptr const& txn, Reply&& reply)
    {
      using request_type = typename message_traits<Reply>::request_type;
      static_cast<transaction<request_type>*>(txn.get())->msg.reply(
          std::forward<Reply>(reply));
      txn->timer.cancel();
    }

    template <class Error>
    static void set_error(transaction_base_ptr const& txn, Error&& error)
    {
      txn->msg_ptr->error(std::forward<Error>(error));
      txn->timer.cancel();
    }


    template <class Channel, class Reply>
    void handle_impl(
          Channel&& channel, Reply&& reply
        , transaction_decorator_detail::is_reply)
    {
      if (auto txn = extract_transaction_from_reply(channel, reply)) {
        auto ctx = txn->context;
        if (ctx.running_in_this_thread()) {
          set_reply(txn, std::move(reply));
        }
        else {
          ctx.post([txn, reply]() mutable { set_reply(txn, std::move(reply)); });
        }
      }
      else {
        this->forward(
            std::forward<Channel>(channel), std::forward<Reply>(reply));
      }
    }

    template <class Channel, class Error>
    void handle_impl(
          Channel&& channel, Error&& error
        , transaction_decorator_detail::is_error)
    {
      if (auto txn = extract_transaction_from_error(channel, error)) {
        auto ctx = txn->context;
        if (ctx.running_in_this_thread()) {
          set_error(txn, std::forward<Error>(error));
        }
        else {
          ctx.post([txn, error]() mutable { set_error(txn, std::move(error)); });
        }
      }
      else {
        this->forward(
            std::forward<Channel>(channel), std::forward<Error>(error));
      }
    }

    template <class Channel, class Message>
    void handle_impl(
          Channel&& channel, Message&& msg
        , transaction_decorator_detail::is_other)
    {
      this->forward(std::forward<Channel>(channel), std::forward<Message>(msg));
    }


    template <class Channel, class Request>
    struct transaction_with_deleter
    {
      transaction_with_deleter(Channel const& channel, std::uint64_t const& key)
        : txn_{channel->get_context()}
        , weak_channel_ptr_{channel}
        , key_(key)
      {}

      ~transaction_with_deleter()
      {
        if (txn_.msg) {
          return;
        }
        if (auto const channel = weak_channel_ptr_.lock()) {
          auto& data = channel->template get_data<transaction_decorator>();
          {
            std::lock_guard<std::mutex> lock{data.mutex};
            data.map.erase(key_);
          }
        }
      }

      transaction<Request> txn_;
      std::weak_ptr<typename Channel::element_type> weak_channel_ptr_;
      std::uint64_t key_;
    };

    template <class Request, class Channel>
    static auto create_transaction(
        Channel const& channel, std::uint64_t const& key)
      -> transaction_ptr<Request>
    {
      auto txn_with_deleter = std::make_shared<
        transaction_with_deleter<Channel, Request>
      >(channel, key);
      return transaction_ptr<Request>{txn_with_deleter, &txn_with_deleter->txn_};
    }

    template <class Channel, class Request>
    static auto register_request(Channel const& channel, Request const& request)
      -> transaction_ptr<Request>
    {
      auto const key = transaction_decorator_detail::to_key(
          Request::type()
        , transaction_decorator_detail::stats_type_if_any<Request>::value
        , request.xid());
      auto txn = create_transaction<Request>(channel, key);
      auto& data = channel->template get_data<transaction_decorator>();
      {
        std::lock_guard<std::mutex> lock{data.mutex};
        data.map.emplace(key, weak_transaction_base_ptr{txn});
        return txn;
      }
    }

    static auto extract_transaction(
          transaction_data& data
        , std::uint16_t const msg_type
        , std::uint16_t const stats_type
        , std::uint32_t const xid)
      -> transaction_base_ptr
    {
      {
        std::lock_guard<std::mutex> lock{data.mutex};
        auto const it = data.map.find(
            transaction_decorator_detail::to_key(msg_type, stats_type, xid));
        if (it == data.map.end()) {
          return {};
        }
        auto const txn_weak_ptr = it->second;
        data.map.erase(it);
        return txn_weak_ptr.lock();
      }
    }

    template <class Channel, class Reply>
    static auto extract_transaction_from_reply(
        Channel const& channel, Reply const& reply)
      -> transaction_base_ptr
    {
      using request_type = typename message_traits<Reply>::request_type;
      constexpr auto stats_type
        = transaction_decorator_detail::stats_type_if_any<request_type>::value;
      return extract_transaction(
            channel->template get_data<transaction_decorator>()
          , request_type::type(), stats_type, reply.xid());
    }

    template <class Channel, class Error>
    static auto extract_transaction_from_error(
        Channel const& channel, Error&& error)
      -> transaction_base_ptr
    {
      auto const data_length = error.data_length();
      if (data_length < sizeof(ofp::v10::protocol::ofp_header)) {
        return {};
      }

      auto const request_header = error.failed_request_header();
      auto stats_type = std::uint16_t{0};
      if (request_header.type == ofp::v10::protocol::msg_type::stats_request) {
        if (data_length < sizeof(ofp::v10::protocol::ofp_stats_request)) {
          return {};
        }
        stats_type = transaction_decorator_detail::get_stats_type(error);
      }

      return extract_transaction(
            channel->template get_data<transaction_decorator>()
          , request_header.type, stats_type, request_header.xid);
    }
  };

} // namespace decorators
} // namespace allium
} // namespace canard

#endif // CANARD_ALLIUM_DECORATORS_TRANSACTION_TRANSACTION_DECORATOR_HPP
