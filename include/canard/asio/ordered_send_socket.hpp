#ifndef CANARD_ASIO_ORDERED_SEND_SOCKET_HPP
#define CANARD_ASIO_ORDERED_SEND_SOCKET_HPP

#include <cstddef>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/asio/detail/consuming_buffers.hpp>
#include <canard/asio/detail/operation_holder.hpp>

namespace canard {

    namespace detail {

        template <class AsyncWriteStream>
        class write_op_base
        {
        public:
            using complete_func_type = void(*)(write_op_base*, boost::system::error_code const, std::size_t);
            using perform_func_type = void(*)(write_op_base*, AsyncWriteStream&, std::size_t);

            write_op_base(std::size_t const buffers_size
                    , complete_func_type const complete_func, perform_func_type const perform_func)
                : complete_func_(complete_func)
                , perform_func_(perform_func)
                , buffers_size_(buffers_size)
            {
            }

            template <class... Args>
            void complete(Args&&... args)
            {
                complete_func_(this, std::forward<Args>(args)...);
            }

            template <class... Args>
            void perform(Args&&... args)
            {
                perform_func_(this, std::forward<Args>(args)...);
            }

            auto buffers_size() const
                -> std::size_t
            {
                return buffers_size_;
            }

        private:
            complete_func_type complete_func_;
            perform_func_type perform_func_;
            std::size_t buffers_size_;
        };

        template <class AsyncWriteStream>
        struct write_op_deleter
        {
            void operator()(write_op_base<AsyncWriteStream>* const op) const
            {
                op->complete(boost::system::error_code{}, 0);
            }
        };

        template <class AsyncWriteStream>
        using write_op_ptr = std::unique_ptr<write_op_base<AsyncWriteStream>, write_op_deleter<AsyncWriteStream>>;

        template <class AsyncWriteStream, class Context>
        class ordered_write_op
        {
        public:
            ordered_write_op(Context context, AsyncWriteStream& stream, std::size_t const already_sent_bytes = 0)
                : context_(std::move(context))
                , stream_(stream)
                , already_sent_bytes_(already_sent_bytes)
            {
            }

            void operator()(boost::system::error_code const ec, std::size_t bytes_transferred)
            {
                stream_.buffers().consume(bytes_transferred);
                auto total_transferred = bytes_transferred + already_sent_bytes_;

                auto it = stream_.handlers().begin();
                for (auto const it_end = stream_.handlers().end(); it != it_end; ++it) {
                    auto const buffers_size = (*it)->buffers_size();
                    if (total_transferred < buffers_size) {
                        break;
                    }
                    (*it).release()->complete(total_transferred == buffers_size ? ec : boost::system::error_code{}, buffers_size);
                    total_transferred -= buffers_size;
                }
                stream_.handlers().erase(stream_.handlers().begin(), it);

                if (ec) {
                    boost::for_each(stream_.handlers(), [&](write_op_ptr<AsyncWriteStream>& handler) {
                        handler.release()->complete(ec, total_transferred);
                        total_transferred -= total_transferred;
                    });
                    stream_.handlers().clear();
                    stream_.buffers().clear();
                    return;
                }

                if (!stream_.handlers().empty()) {
                    stream_.handlers().front()->perform(stream_, total_transferred);
                }
            }

            template <class Function>
            friend void asio_handler_invoke(Function&& function, ordered_write_op* const context)
            {
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(std::forward<Function>(function), std::addressof(context->context_));
            }

            friend auto asio_handler_allocate(std::size_t const size, ordered_write_op* const context)
                -> void*
            {
                using boost::asio::asio_handler_allocate;
                return asio_handler_allocate(size, std::addressof(context->context_));
            }

            friend void asio_handler_deallocate(void* const pointer, std::size_t const size, ordered_write_op* const context)
            {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(pointer, size, std::addressof(context->context_));
            }

            friend auto asio_handler_is_continuation(ordered_write_op*)
                -> bool
            {
                return true;
            }

        private:
            Context context_;
            AsyncWriteStream& stream_;
            std::size_t already_sent_bytes_;
        };

        template <class AsyncWriteStream, class WriteHandler>
        class write_op
            : public write_op_base<AsyncWriteStream>
        {
        public:
            explicit write_op(WriteHandler handler, std::size_t const buffers_size)
                : write_op_base<AsyncWriteStream>{buffers_size, &write_op::complete, &write_op::perform}
                , handler_(std::move(handler))
            {
            }

        private:
            static void complete(write_op_base<AsyncWriteStream>* const op, boost::system::error_code ec, std::size_t bytes_transferred)
            {
                auto const this_op = static_cast<write_op*>(op);
                auto holder = detail::op_holder<WriteHandler, write_op>{this_op->handler_, this_op};
                auto function = detail::bind(std::move(this_op->handler_), ec, bytes_transferred);
                holder.handler(function.handler());
                holder.reset();

                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
            }

            static void perform(write_op_base<AsyncWriteStream>* const op, AsyncWriteStream& stream, std::size_t already_sent_bytes)
            {
                auto const this_op = static_cast<write_op*>(op);
                boost::asio::async_write(stream.next_layer(), stream.buffers(), boost::asio::transfer_at_least(this_op->buffers_size() - already_sent_bytes)
                        , ordered_write_op<AsyncWriteStream, WriteHandler>{this_op->handler_, stream, already_sent_bytes});
            }

        private:
            WriteHandler handler_;
        };

    } // namespace detail

    template <class Socket = boost::asio::ip::tcp::socket>
    class ordered_send_socket
    {
    public:
        using next_layer_type = typename std::remove_reference<Socket>::type;
        using lowest_layer_type = typename next_layer_type::lowest_layer_type;
        using native_handle_type = typename next_layer_type::native_handle_type;

        explicit ordered_send_socket(boost::asio::io_service& io_service)
            : socket_{io_service}
        {
        }

        explicit ordered_send_socket(Socket socket)
            : socket_(std::move(socket))
        {
        }

        auto native_handle()
            -> native_handle_type
        {
            return socket_.native_handle();
        }

        auto next_layer()
            -> next_layer_type&
        {
            return socket_;
        }

        auto lowest_layer()
            -> lowest_layer_type&
        {
            return socket_.lowest_layer();
        }

        template <class MutableBufferSequence, class ReadHandler>
        auto async_read_some(MutableBufferSequence&& buffers, ReadHandler handler)
            -> typename async_result_init<ReadHandler, void(boost::system::error_code, std::size_t)>::result_type
        {
            return next_layer().async_read_some(std::forward<MutableBufferSequence>(buffers), std::move(handler));
        }

        template <class ConstBufferSequence, class WriteHandler>
        auto async_write_some(ConstBufferSequence&& buffers, WriteHandler handler)
            -> typename async_result_init<WriteHandler, void(boost::system::error_code, std::size_t)>::result_type
        {
            using handler_type = typename async_result_init<
                    WriteHandler, void(boost::system::error_code, std::size_t)
            >::handler_type;

            auto init = async_result_init<WriteHandler, void(boost::system::error_code, std::size_t)>{std::move(handler)};

            auto const is_already_sending = !write_handlers_.empty();

            using operation_type = detail::write_op<ordered_send_socket, handler_type>;
            auto holder = detail::op_holder<handler_type, operation_type>{init.handler()};

            write_handlers_.emplace_back(holder.construct(init.handler(), boost::asio::buffer_size(buffers)));
            boost::push_back(buffers_, buffers);

            if (!is_already_sending) {
                boost::asio::async_write(next_layer(), buffers_
                        , detail::ordered_write_op<ordered_send_socket, handler_type>{init.handler(), *this});
            }

            holder.release();

            return init.get();
        }

    // private:

        auto buffers()
            -> detail::consuming_buffers<boost::asio::const_buffer>&
        {
            return buffers_;
        }

        auto handlers()
            -> std::vector<detail::write_op_ptr<ordered_send_socket>>&
        {
            return write_handlers_;
        }

    private:
        Socket socket_;
        detail::consuming_buffers<boost::asio::const_buffer> buffers_;
        std::vector<detail::write_op_ptr<ordered_send_socket>> write_handlers_;
    };

} // namespace canard

#endif // CANARD_ASIO_ORDERED_SEND_SOCKET_HPP
