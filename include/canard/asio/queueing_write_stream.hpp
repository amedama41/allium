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
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/detail/consuming_buffers.hpp>
#include <boost/asio/detail/op_queue.hpp>
#include <boost/asio/detail/operation.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/asio/detail/operation_holder.hpp>
#include <canard/exception.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace detail {

    class gather_buffers
    {
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
        auto gather(Queue& waiting_queue)
            -> gather_buffers&
        {
            buffers_->clear();
            for (auto op = waiting_queue.front();
                    op;
                    op = boost::asio::detail::op_queue_access::next(op)) {
                op->buffers(*buffers_);
            }
            return *this;
        }

    private:
        std::shared_ptr<std::vector<value_type>> buffers_;
    };

    template <class Stream>
    class write_op_base
        : public boost::asio::detail::operation
    {
    protected:
        using complete_func_type = boost::asio::detail::operation::func_type;
        using perform_func_type
            = void(*)(write_op_base*, Stream&, gather_buffers);
        using consume_func_type = void(*)(write_op_base*, std::size_t);
        using buffers_func_type
            = void(*)(write_op_base*, std::vector<boost::asio::const_buffer>&);

        write_op_base(
                  complete_func_type const complete_func
                , perform_func_type const perform_func
                , consume_func_type const consume_func
                , buffers_func_type const buffers_func
                , std::size_t const buffers_size)
            : boost::asio::detail::operation{complete_func}
            , perform_func_(perform_func)
            , consume_func_(consume_func)
            , buffers_func_(buffers_func)
            , buffers_size_(buffers_size)
            , ec_{}
            , total_transferred_{0}
        {
        }

    public:
        void perform(Stream& stream, gather_buffers buffers)
        {
            perform_func_(this, stream, std::move(buffers));
        }

        auto consume(std::size_t& bytes_transferred)
            -> std::size_t
        {
            auto const consumed_size = std::min(bytes_transferred, buffers_size());
            consume_func_(this, bytes_transferred);
            bytes_transferred -= consumed_size;
            total_transferred_ += consumed_size;
            return buffers_size();
        }

        void buffers(std::vector<boost::asio::const_buffer>& buffers)
        {
            buffers_func_(this, buffers);
        }

        auto buffers_size() const
            -> std::size_t
        {
            return buffers_size_ - total_transferred_;
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

        auto bytes_transferred() const
            -> std::size_t
        {
            return total_transferred_;
        }

    private:
        perform_func_type perform_func_;
        consume_func_type consume_func_;
        buffers_func_type buffers_func_;
        std::size_t buffers_size_;
        boost::system::error_code ec_;
        std::size_t total_transferred_;
    };

    template <class Stream, class Context>
    class queueing_write_handler
    {
    private:
        using write_op_queue = boost::asio::detail::op_queue<write_op_base<Stream>>;
        using operation_queue
            = boost::asio::detail::op_queue<boost::asio::detail::operation>;

        struct on_do_complete_exit
        {
            ~on_do_complete_exit()
            {
                if (auto const write_op = handler_.waiting_queue_->front()) {
                    try {
                        write_op->perform(
                                  handler_.stream_
                                , handler_.stream_.buffers_.gather(*handler_.waiting_queue_));
                    }
                    catch (boost::system::system_error const& e) {
                        set_error_code(e.code());
                    }
                    catch (std::system_error const& e) {
                        set_error_code(boost::system::error_code{
                                e.code().value(), boost::system::system_category()
                        });
                    }
                    catch (std::bad_alloc const&) {
                        set_error_code(make_error_code(
                                    boost::system::errc::not_enough_memory));
                    }
                    catch (std::exception const&) {
                        set_error_code(make_error_code(canard::has_any_exception));
                    }
                }
                if (!ready_queue_.empty()) {
                    handler_.io_service_.post_deferred_completions(ready_queue_);
                }
            }

            void set_error_code(boost::system::error_code const& ec)
            {
                while (auto const op = handler_.waiting_queue_->front()) {
                    handler_.waiting_queue_->pop();
                    op->error_code(ec);
                    ready_queue_.push(op);
                }
            }

            queueing_write_handler& handler_;
            operation_queue& ready_queue_;
        };

    public:
        queueing_write_handler(Stream& stream, Context* const context)
            : stream_(stream)
            , waiting_queue_(stream_.waiting_queue_)
            , context_(context)
            , io_service_(boost::asio::use_service<
                    boost::asio::detail::io_service_impl
              >(stream_.get_io_service()))
        {
        }

        void operator()(boost::system::error_code const ec, std::size_t bytes_transferred)
        {
            operation_queue ready_queue{};
            while (auto const op = waiting_queue_->front()) {
                if (op->consume(bytes_transferred) != 0) {
                    break;
                }
                if (bytes_transferred == 0) {
                    op->error_code(ec);
                }
                waiting_queue_->pop();
                ready_queue.push(op);
            }

            if (ec) {
                while (auto const op = waiting_queue_->front()) {
                    waiting_queue_->pop();
                    op->error_code(ec);
                    ready_queue.push(op);
                }
            }

            on_do_complete_exit on_exit{*this, ready_queue};

            while (auto const op = ready_queue.front()) {
                ready_queue.pop();
                op->complete(io_service_, ec, 0);
            }
        }

        template <class Function>
        friend void asio_handler_invoke(
                Function&& function, queueing_write_handler* const handler)
        {
            using boost::asio::asio_handler_invoke;
            asio_handler_invoke(
                      std::forward<Function>(function)
                    , std::addressof(handler->context_->handler()));
        }

        friend auto asio_handler_allocate(
                std::size_t const size, queueing_write_handler* const handler)
            -> void*
        {
            using boost::asio::asio_handler_allocate;
            return asio_handler_allocate(
                    size, std::addressof(handler->context_->handler()));
        }

        friend void asio_handler_deallocate(
                  void* const pointer, std::size_t const size
                , queueing_write_handler* const handler)
        {
            using boost::asio::asio_handler_deallocate;
            asio_handler_deallocate(
                    pointer, size, std::addressof(handler->context_->handler()));
        }

        friend auto asio_handler_is_continuation(queueing_write_handler*)
            -> bool
        {
            return true;
        }

    private:
        Stream& stream_;
        std::shared_ptr<write_op_queue> waiting_queue_;
        Context* context_;
        boost::asio::detail::io_service_impl& io_service_;
    };

    template <class Stream, class WriteHandler, class ConstBufferSequence>
    class waiting_op
        : public write_op_base<Stream>
    {
    public:
        waiting_op(WriteHandler handler, ConstBufferSequence const& buffers)
            : write_op_base<Stream>{
                  &do_complete, &do_perform, &do_consume, &do_buffers
                , boost::asio::buffer_size(buffers)
              }
            , handler_(std::move(handler))
            , buffers_(buffers)
        {
        }

        auto handler()
            -> WriteHandler&
        {
            return handler_;
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
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
            }
        }

        static void do_perform(
                  write_op_base<Stream>* base, Stream& stream
                , gather_buffers buffers)
        {
            auto const op = static_cast<waiting_op*>(base);
            stream.next_layer().async_write_some(
                      std::move(buffers)
                    , queueing_write_handler<Stream, waiting_op>{stream, op});
        }

        static void do_consume(
                write_op_base<Stream>* base, std::size_t bytes_transferred)
        {
            auto const op = static_cast<waiting_op*>(base);
            op->buffers_.consume(bytes_transferred);
        }

        static void do_buffers(
                  write_op_base<Stream>* base
                , std::vector<boost::asio::const_buffer>& buffers)
        {
            auto const op = static_cast<waiting_op*>(base);
            boost::push_back(buffers, op->buffers_);
        }

    private:
        WriteHandler handler_;
        boost::asio::detail::consuming_buffers<
            boost::asio::const_buffer, ConstBufferSequence
        > buffers_;
    };

} // namespace detail

template <class Stream = boost::asio::ip::tcp::socket>
class queueing_write_stream
{
private:
    using write_op_queue = boost::asio::detail::op_queue<
        detail::write_op_base<queueing_write_stream>
    >;

    using read_handler_type = void(boost::system::error_code, std::size_t);
    template <class ReadHandler>
    using read_result_init = async_result_init<
        canard::remove_cv_and_reference_t<ReadHandler>, read_handler_type
    >;
    using write_handler_type = void(boost::system::error_code, std::size_t);
    template <class WriteHandler>
    using write_result_init = async_result_init<
        canard::remove_cv_and_reference_t<WriteHandler>, write_handler_type
    >;

    struct queue_cleanup
    {
        ~queue_cleanup()
        {
            if (!commit) {
                queue_->pop();
            }
        }
        write_op_queue* queue_;
        bool commit;
    };

public:
    using next_layer_type = typename std::remove_reference<Stream>::type;
    using lowest_layer_type = typename next_layer_type::lowest_layer_type;
    using native_handle_type = typename next_layer_type::native_handle_type;

    explicit queueing_write_stream(boost::asio::io_service& io_service)
        : stream_{io_service}
        , waiting_queue_{std::make_shared<write_op_queue>()}
    {
    }

    explicit queueing_write_stream(Stream stream)
        : stream_(std::move(stream))
        , waiting_queue_{std::make_shared<write_op_queue>()}
    {
    }

    auto get_io_service()
        -> boost::asio::io_service&
    {
        return next_layer().get_io_service();
    }

    auto native_handle()
        -> native_handle_type
    {
        return stream_.native_handle();
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
        auto init = write_result_init<WriteHandler>{
            std::forward<WriteHandler>(handler)
        };

        using handler_type
            = typename write_result_init<WriteHandler>::handler_type;

        using operation_type = detail::waiting_op<
              queueing_write_stream, handler_type
            , canard::remove_cv_and_reference_t<ConstBufferSequence>
        >;

        detail::op_holder<handler_type, operation_type> holder{
            init.handler()
        };
        auto const write_op = holder.construct(
                init.handler(), std::forward<ConstBufferSequence>(buffers));

        auto const enable_to_send = waiting_queue_->empty();
        waiting_queue_->push(write_op);
        if (enable_to_send) {
            queue_cleanup on_exit{waiting_queue_.get(), false};
            write_op->perform(*this, buffers_.gather(*waiting_queue_));
            on_exit.commit = true;
        }

        holder.release();

        return init.get();
    }

private:
    template <class, class> friend class detail::queueing_write_handler;
    Stream stream_;
    std::shared_ptr<write_op_queue> waiting_queue_;
    detail::gather_buffers buffers_;
};

} // namespace canard

#endif // CANARD_ASIO_QUEUEING_WRITE_STREAM_HPP

