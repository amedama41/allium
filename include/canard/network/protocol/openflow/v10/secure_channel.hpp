#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP

#include <cstddef>
#include <memory>
#include <utility>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/queueing_write_stream.hpp>
#include <canard/asio/suppress_asio_async_result_propagation.hpp>
#include <canard/network/protocol/openflow/detail/null_handler.hpp>
#include <canard/network/protocol/openflow/shared_buffer_generator.hpp>
#include <canard/network/protocol/openflow/with_buffer.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    template <class Socket = boost::asio::ip::tcp::socket>
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
            async_write_result_init<WriteHandler> init{
                std::forward<WriteHandler>(handler)
            };
            strand_.dispatch(
                    make_async_write_functor(
                          this->shared_from_this()
                        , std::move(init.handler())
                        , msg.encode()));
            return init.get();
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
        {
            void operator()()
            {
                channel_->async_write_some(
                          std::move(buffers_)
                        , canard::suppress_asio_async_result_propagation(
                              std::move(handler_)));
            }

            friend auto asio_handler_allocate(
                    std::size_t const size, async_write_functor* const func)
                -> void*
            {
                using boost::asio::asio_handler_allocate;
                return asio_handler_allocate(
                        size, std::addressof(func->handler_));
            }

            friend void asio_handler_deallocate(
                    void* const pointer, std::size_t const size
                    , async_write_functor* const func)
            {
                using boost::asio::asio_handler_deallocate;
                asio_handler_deallocate(
                        pointer, size, std::addressof(func->handler_));
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
        canard::queueing_write_stream<
            Socket, boost::asio::io_service::strand
        > stream_;
        boost::asio::io_service::strand strand_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
