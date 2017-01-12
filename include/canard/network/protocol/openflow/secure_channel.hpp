#ifndef CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_HPP

#include <cstddef>
#include <memory>
#include <utility>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/asio_handler_hook_propagation.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/suppress_asio_async_result_propagation.hpp>
#include <canard/asio/write_queue_stream.hpp>
#include <canard/network/protocol/openflow/decorator.hpp>
#include <canard/network/protocol/openflow/detail/null_handler.hpp>
#include <canard/network/protocol/openflow/shared_buffer_generator.hpp>
#include <canard/network/protocol/openflow/with_buffer.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {

    namespace detail {

        template <class ChannelDataMap, class Socket>
        class secure_channel_with_data;

    } // namespace detail

    template <class Socket>
    class secure_channel
        : public std::enable_shared_from_this<secure_channel<Socket>>
    {
        template <class WriteHandler>
        using async_write_result_init = canard::async_result_init<
              canard::remove_cv_and_reference_t<WriteHandler>
            , void(boost::system::error_code, std::size_t)
        >;

    public:
        secure_channel(
                  Socket socket
                , boost::asio::io_service::strand strand)
            : stream_{std::move(socket), strand}
            , strand_{std::move(strand)}
        {
        }

        auto get_io_service()
            -> boost::asio::io_service&
        {
            return stream_.get_io_service();
        }

        auto get_context()
            -> boost::asio::io_service::strand
        {
            return strand_;
        }

        template <class T>
        auto get_data()
            -> detail::channel_data_t<T, detail::channel_data_map_t<T>>
        {
            return static_cast<detail::secure_channel_with_data<
                detail::channel_data_map_t<T>, Socket
            >*>(this)->template get_channel_data<T>();
        }

        template <class T>
        auto get_data() const
            -> detail::channel_data_t<T, detail::channel_data_map_t<T> const>
        {
            return static_cast<detail::secure_channel_with_data<
                detail::channel_data_map_t<T>, Socket
            > const*>(this)->template get_channel_data<T>();
        }

        void close()
        {
            auto channel = this->shared_from_this();
            strand_.dispatch([channel]{
                if (channel->stream_.lowest_layer().is_open()) {
                    auto ignore = boost::system::error_code{};
                    channel->stream_.lowest_layer().close(ignore);
                }
            });
        }

        template <class Message, class Buffer, class WriteHandler>
        auto async_send(
                  detail::message_with_buffer<Message, Buffer> const& msg
                , WriteHandler&& handler)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            if (strand_.running_in_this_thread()) {
                return async_write_some(
                        msg.encode(), std::forward<WriteHandler>(handler));
            }
            else {
                async_write_result_init<WriteHandler> init{
                    std::forward<WriteHandler>(handler)
                };
                strand_.post(make_async_write_functor(
                              this->shared_from_this()
                            , std::move(init.handler()), msg.encode()));
                return init.get();
            }
        }

        template <class Message, class WriteHandler>
        auto async_send(Message const& msg, WriteHandler&& handler)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            return async_send(with_buffer(msg, shared_buffer_generator{})
                      , std::forward<WriteHandler>(handler));
        }

        template <class Message>
        auto async_send(Message const& msg)
            -> typename async_write_result_init<detail::null_handler>::result_type
        {
            return async_send(msg, detail::null_handler{});
        }

    protected:
        template <class ConstBufferSequence, class WriteHandler>
        auto async_write_some(
                ConstBufferSequence&& buffers, WriteHandler&& handler)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            return stream_.async_write_some(
                      std::forward<ConstBufferSequence>(buffers)
                    , std::forward<WriteHandler>(handler));
        }

    private:
        template <class WriteHandler, class ConstBufferSequence>
        struct async_write_functor
            : canard::asio_handler_hook_propagation<
                  async_write_functor<WriteHandler, ConstBufferSequence>
                , canard::no_propagation_hook_invoke
              >
        {
            template <class Channel, class Handler, class BufferSequence>
            async_write_functor(Channel&& c, Handler&& h, BufferSequence&& b)
                : channel_(std::forward<Channel>(c))
                , handler_(std::forward<Handler>(h))
                , buffers_(std::forward<BufferSequence>(b))
            {
            }

            void operator()()
            {
                channel_->async_write_some(
                          std::move(buffers_)
                        , canard::suppress_asio_async_result_propagation(
                              std::move(handler_)));
            }

            auto handler() noexcept
                -> WriteHandler&
            {
                return handler_;
            }

            std::shared_ptr<secure_channel> channel_;
            WriteHandler handler_;
            ConstBufferSequence buffers_;
        };

        template <class WriteHandler, class ConstBufferSequence>
        static auto make_async_write_functor(
                  std::shared_ptr<secure_channel>&& c
                , WriteHandler&& h
                , ConstBufferSequence&& cb)
            -> async_write_functor<
                      canard::remove_cv_and_reference_t<WriteHandler>
                    , canard::remove_cv_and_reference_t<ConstBufferSequence>
               >
        {
            return async_write_functor<
                  canard::remove_cv_and_reference_t<WriteHandler>
                , canard::remove_cv_and_reference_t<ConstBufferSequence>
            >{
                  std::move(c), std::forward<WriteHandler>(h)
                , std::forward<ConstBufferSequence>(cb)
            };
        }

    protected:
        canard::write_queue_stream<
            Socket, boost::asio::io_service::strand
        > stream_;
        boost::asio::io_service::strand strand_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_SECURE_CHANNEL_HPP
