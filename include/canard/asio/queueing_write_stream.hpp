#ifndef CANARD_ASIO_QUEUEING_WRITE_STREAM_HPP
#define CANARD_ASIO_QUEUEING_WRITE_STREAM_HPP

#include <cstddef>
#include <type_traits>
#include <memory>
#include <new>
#include <system_error>
#include <utility>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/detail/buffer_sequence_adapter.hpp>
#include <boost/asio/detail/fenced_block.hpp>
#include <boost/asio/detail/op_queue.hpp>
#include <boost/asio/detail/operation.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/asio/detail/consuming_buffers.hpp>
#include <canard/asio/detail/operation_holder.hpp>
#include <canard/exception.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace detail {

    class write_op_base
        : public boost::asio::detail::operation
    {
    protected:
        using complete_func_type = boost::asio::detail::operation::func_type;
        using consume_func_type
            = auto(*)(write_op_base*, std::size_t&)
                -> boost::asio::const_buffer;
        using buffers_func_type
            = void(*)(write_op_base*, std::vector<boost::asio::const_buffer>&);

        write_op_base(
                  complete_func_type const complete_func
                , consume_func_type const consume_func
                , buffers_func_type const buffers_func)
            : boost::asio::detail::operation{complete_func}
            , consume_func_(consume_func)
            , buffers_func_(buffers_func)
            , ec_{}
            , bytes_transferred_{}
        {
        }

    public:
        auto consume(std::size_t& bytes_transferred)
            -> boost::asio::const_buffer
        {
            return consume_func_(this, bytes_transferred);
        }

        void buffers(std::vector<boost::asio::const_buffer>& buffers)
        {
            buffers_func_(this, buffers);
        }

        auto error_code() const
            -> boost::system::error_code const&
        {
            return ec_;
        }

        void error_code(boost::system::error_code const ec)
        {
            ec_ = ec;
        }

        auto bytes_transferred() const noexcept
            -> std::size_t
        {
            return bytes_transferred_;
        }

        auto bytes_transferred_ref() noexcept
            -> std::size_t&
        {
            return bytes_transferred_;
        }

        void add_bytes_transferred(std::size_t const bytes) noexcept
        {
            bytes_transferred_ += bytes;
        }

    private:
        consume_func_type consume_func_;
        buffers_func_type buffers_func_;
        boost::system::error_code ec_;
        std::size_t bytes_transferred_;
    };

    template <class WriteHandler, class ConstBufferSequence>
    class waiting_op
        : public write_op_base
    {
    public:
        waiting_op(WriteHandler& handler, ConstBufferSequence&& buffers)
            : write_op_base{&do_complete, &do_consume, &do_buffers}
            , handler_(handler)
            , buffers_(std::move(buffers))
        {
        }

        waiting_op(WriteHandler& handler, ConstBufferSequence const& buffers)
            : write_op_base{&do_complete, &do_consume, &do_buffers}
            , handler_(handler)
            , buffers_(buffers)
        {
        }

    private:
        static void do_complete(
                  boost::asio::detail::io_service_impl* owner
                , boost::asio::detail::operation* base
                , boost::system::error_code const& ec
                , std::size_t bytes_transferred)
        {
            auto const op = static_cast<waiting_op*>(base);

            detail::op_holder<WriteHandler, waiting_op> holder{
                op->handler_, op
            };

            auto function = detail::bind(
                    op->handler_, op->error_code(), op->bytes_transferred());

            holder.handler(function.handler());
            holder.reset();

            if (owner)
            {
                boost::asio::detail::fenced_block b{
                    boost::asio::detail::fenced_block::half
                };
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
            }
        }

        static auto do_consume(
                write_op_base* base, std::size_t& bytes_transferred)
            -> boost::asio::const_buffer
        {
            auto const op = static_cast<waiting_op*>(base);
            return op->buffers_.consume(
                    op->bytes_transferred_ref(), bytes_transferred);
        }

        static void do_buffers(
                  write_op_base* base
                , std::vector<boost::asio::const_buffer>& buffers)
        {
            auto const op = static_cast<waiting_op*>(base);
            op->buffers_.push_back_to(buffers);
        }

    private:
        WriteHandler handler_;
        canard::detail::consuming_buffers<ConstBufferSequence> buffers_;
    };

    class gather_buffers
    {
        struct asio_limiting_params
            : private boost::asio::detail::buffer_sequence_adapter_base
        {
            using buffer_sequence_adapter_base::max_buffers;
        };

    public:
        using value_type = boost::asio::const_buffer;
        using const_iterator = std::vector<value_type>::const_iterator;

        gather_buffers()
            : buffers_(std::make_shared<std::vector<value_type>>())
        {
        }

        auto begin() const
            -> const_iterator
        {
            return buffers_->begin();
        }

        auto end() const
            -> const_iterator
        {
            return buffers_->end();
        }

        template <class Queue>
        auto gather(boost::asio::const_buffer const& buffer
                  , Queue& waiting_queue)
            -> gather_buffers&
        {
            buffers_->clear();
            buffers_->push_back(buffer);
            gather_impl(waiting_queue);
            return *this;
        }

        template <class Queue>
        auto gather(Queue& waiting_queue)
            -> gather_buffers&
        {
            buffers_->clear();
            gather_impl(waiting_queue);
            return *this;
        }

        template <class Queue>
        void gather_impl(Queue& waiting_queue)
        {
            for (auto op = waiting_queue.front();
                    op;
                    op = boost::asio::detail::op_queue_access::next(op)) {
                op->buffers(*buffers_);
                if (buffers_->size() >= asio_limiting_params::max_buffers) {
                    break;
                }
            }
        }

    private:
        std::shared_ptr<std::vector<value_type>> buffers_;
    };

    template <class Stream, class Context>
    class queueing_write_handler;

    template <class Context>
    struct queueing_write_stream_impl
        : Context
    {
        explicit queueing_write_stream_impl(Context&& context)
            : Context(std::move(context))
        {
        }

        template <class Stream>
        void start_write(
                  Stream& stream
                , std::shared_ptr<queueing_write_stream_impl> ptr)
        {
            head_buffer = boost::asio::const_buffer{};

            auto& io_service_impl = boost::asio::use_service<
                boost::asio::detail::io_service_impl
            >(stream.get_io_service());

            stream.next_layer().async_write_some(
                      buffers_.gather(waiting_queue)
                    , queueing_write_handler<Stream, Context>{
                            stream, io_service_impl, std::move(ptr)
                      });
        }

        template <class Stream>
        void continue_write(Stream& stream
                    , queueing_write_handler<Stream, Context> const& handler)
        {
            stream.next_layer().async_write_some(
                    buffers_.gather(head_buffer, waiting_queue), handler);
        }

        boost::asio::detail::op_queue<detail::write_op_base> waiting_queue;
        boost::asio::const_buffer head_buffer;
        gather_buffers buffers_;
    };

    void set_error_code(
              boost::system::error_code const& ec
            , boost::asio::detail::op_queue<detail::write_op_base>& queue)
    {
        for (auto op = queue.front();
             op;
             op = boost::asio::detail::op_queue_access::next(op)) {
            op->error_code(ec);
        }
    }

    template <class Stream, class Context>
    class queueing_write_handler
    {
    private:
        using operation_queue
            = boost::asio::detail::op_queue<boost::asio::detail::operation>;

        struct on_do_complete_exit
        {
            ~on_do_complete_exit()
            {
                if (need_write) {
                    try {
                        this_->impl_->continue_write(this_->stream_, *this_);
                    }
                    catch (boost::system::system_error const& e) {
                        set_error_to_ready_queue(e.code());
                    }
                    catch (std::system_error const& e) {
                        set_error_to_ready_queue(boost::system::error_code{
                                  e.code().value()
                                , boost::system::system_category()
                        });
                    }
                    catch (std::exception const&) {
                        set_error_to_ready_queue(
                                make_error_code(canard::has_any_exception));
                    }
                }

                while (auto const op = ready_queue.front()) {
                    ready_queue.pop();
                    this_->io_service_impl_.post_immediate_completion(op, true);
                }
            }

            void set_error_to_ready_queue(boost::system::error_code const& ec)
            {
                set_error_code(ec, this_->impl_->waiting_queue);
                ready_queue.push(this_->impl_->waiting_queue);
            }

            queueing_write_handler* this_;
            operation_queue& ready_queue;
            bool need_write;
        };

    public:
        queueing_write_handler(
                  Stream& stream
                , boost::asio::detail::io_service_impl& io_service_impl
                , std::shared_ptr<queueing_write_stream_impl<Context>>&& impl)
            : stream_(stream)
            , io_service_impl_(io_service_impl)
            , impl_(std::move(impl))
        {
        }

        void operator()(boost::system::error_code const ec
                      , std::size_t bytes_transferred)
        {
            operation_queue ready_queue{};

            auto const head_op = impl_->waiting_queue.front();
            auto const head_buffer_size
                = boost::asio::buffer_size(impl_->head_buffer);
            if (head_buffer_size <= bytes_transferred) {
                head_op->add_bytes_transferred(head_buffer_size);
                bytes_transferred -= head_buffer_size;
                while (auto const op = impl_->waiting_queue.front()) {
                    auto const buffer = op->consume(bytes_transferred);
                    if (boost::asio::buffer_size(buffer) != 0) {
                        impl_->head_buffer = buffer;
                        break;
                    }
                    if (bytes_transferred == 0) {
                        op->error_code(ec);
                    }
                    impl_->waiting_queue.pop();
                    ready_queue.push(op);
                }
            }
            else {
                impl_->head_buffer = impl_->head_buffer + bytes_transferred;
                head_op->add_bytes_transferred(bytes_transferred);
            }

            if (ec) {
                set_error_code(ec, impl_->waiting_queue);
                ready_queue.push(impl_->waiting_queue);
            }

            on_do_complete_exit on_exit{
                this, ready_queue, !impl_->waiting_queue.empty()
            };

            while (auto const op = ready_queue.front()) {
                ready_queue.pop();
                op->complete(io_service_impl_, ec, 0);
            }
        }

        template <class Function>
        friend void asio_handler_invoke(
                Function&& function, queueing_write_handler* const handler)
        {
            using boost::asio::asio_handler_invoke;
            asio_handler_invoke(
                      std::forward<Function>(function)
                    , static_cast<Context*>(handler->impl_.get()));
        }

        friend auto asio_handler_allocate(
                std::size_t const size, queueing_write_handler* const handler)
            -> void*
        {
            using boost::asio::asio_handler_allocate;
            return asio_handler_allocate(
                      size, static_cast<Context*>(handler->impl_.get()));
        }

        friend void asio_handler_deallocate(
                  void* const pointer, std::size_t const size
                , queueing_write_handler* const handler)
        {
            using boost::asio::asio_handler_deallocate;
            asio_handler_deallocate(
                      pointer, size
                    , static_cast<Context*>(handler->impl_.get()));
        }

        friend auto asio_handler_is_continuation(queueing_write_handler*)
            -> bool
        {
            return true;
        }

    private:
        Stream& stream_;
        boost::asio::detail::io_service_impl& io_service_impl_;
        std::shared_ptr<queueing_write_stream_impl<Context>> impl_;
    };

    struct null_context
    {
        void operator()() const {}
    };

    template <class Context>
    struct queueing_stream_context
    {
        using type = Context;

        static auto convert(Context& context)
            -> Context&&
        {
            return std::move(context);
        }
    };

    template <>
    struct queueing_stream_context<boost::asio::io_service::strand>
    {
        using original_context = boost::asio::io_service::strand;
        using type = decltype(
                std::declval<original_context>().wrap(null_context{}));

        static auto convert(original_context& strand)
            -> type
        {
            return strand.wrap(null_context{});
        }
    };

    template <>
    struct queueing_stream_context<boost::asio::io_service>
    {
        using original_context = boost::asio::io_service;
        using type = decltype(
                std::declval<original_context>().wrap(null_context{}));

        static auto convert(original_context& io_service)
            -> type
        {
            return io_service.wrap(null_context{});
        }
    };

} // namespace detail

template <
      class Stream = boost::asio::ip::tcp::socket
    , class Context = detail::null_context
>
class queueing_write_stream
{
private:
    using context_helper = detail::queueing_stream_context<Context>;
    using impl_type = detail::queueing_write_stream_impl<
        typename context_helper::type
    >;

    template <class ReadHandler>
    using read_result_init = async_result_init<
          canard::remove_cv_and_reference_t<ReadHandler>
        , void(boost::system::error_code, std::size_t)
    >;

    template <class WriteHandler>
    using write_result_init = async_result_init<
          canard::remove_cv_and_reference_t<WriteHandler>
        , void(boost::system::error_code, std::size_t)
    >;

    template <class CompletionHandler>
    using completion_result_init = async_result_init<
          canard::remove_cv_and_reference_t<CompletionHandler>
        , void()
    >;

    struct queue_cleanup
    {
        ~queue_cleanup()
        {
            if (!commit) {
                queue->pop();
            }
        }
        boost::asio::detail::op_queue<detail::write_op_base>* queue;
        bool commit;
    };

public:
    using next_layer_type = typename std::remove_reference<Stream>::type;
    using lowest_layer_type = typename next_layer_type::lowest_layer_type;

    explicit queueing_write_stream(Stream stream)
        : queueing_write_stream{std::move(stream), Context{}}
    {
    }

    queueing_write_stream(Stream stream, Context context)
        : stream_(std::move(stream))
        , impl_{std::make_shared<impl_type>(context_helper::convert(context))}
    {
    }

    template <class... Args>
    explicit queueing_write_stream(
              boost::asio::io_service& io_service
            , Args&&... args)
        : queueing_write_stream{
              Context{}
            , io_service, std::forward<Args>(args)...
          }
    {
    }

    template <class... Args>
    queueing_write_stream(Context context, Args&&... args)
        : stream_{std::forward<Args>(args)...}
        , impl_{std::make_shared<impl_type>(context_helper::convert(context))}
    {
    }

    auto get_io_service()
        -> boost::asio::io_service&
    {
        return next_layer().get_io_service();
    }

    auto next_layer()
        -> next_layer_type&
    {
        return stream_;
    }

    auto lowest_layer()
        -> lowest_layer_type&
    {
        return stream_.lowest_layer();
    }

    template <class MutableBufferSequence, class ReadHandler>
    auto async_read_some(MutableBufferSequence&& buffers, ReadHandler&& handler)
        -> typename read_result_init<ReadHandler>::result_type
    {
        return next_layer().async_read_some(
                  std::forward<MutableBufferSequence>(buffers)
                , std::forward<ReadHandler>(handler));
    }

    template <class ConstBufferSequence, class WriteHandler>
    auto async_write_some(ConstBufferSequence&& buffers, WriteHandler&& handler)
        -> typename write_result_init<WriteHandler>::result_type
    {
        write_result_init<WriteHandler> init{
            std::forward<WriteHandler>(handler)
        };

        using handler_type
            = typename write_result_init<WriteHandler>::handler_type;

        using operation_type = detail::waiting_op<
              handler_type
            , canard::remove_cv_and_reference_t<ConstBufferSequence>
        >;

        detail::op_holder<handler_type, operation_type> holder{
            init.handler()
        };
        auto const write_op = holder.construct(
                init.handler(), std::forward<ConstBufferSequence>(buffers));

        auto const enable_to_send = impl_->waiting_queue.empty();
        impl_->waiting_queue.push(write_op);
        if (enable_to_send) {
            queue_cleanup on_exit{
                std::addressof(impl_->waiting_queue), false
            };
            impl_->start_write(*this, impl_);
            on_exit.commit = true;
        }

        holder.release();

        return init.get();
    }

private:
    Stream stream_;
    std::shared_ptr<impl_type> impl_;
};

} // namespace canard

#endif // CANARD_ASIO_QUEUEING_WRITE_STREAM_HPP

