#ifndef CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
#define CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/system/error_code.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/asio/async_result_init.hpp>
#include <canard/asio/queueing_write_stream.hpp>
#include <canard/asio/shared_buffer.hpp>
#include <canard/asio/suppress_asio_async_result_propagation.hpp>
#include <canard/network/protocol/openflow/detail/buffer_sequence_adaptor.hpp>
#include <canard/network/protocol/openflow/detail/null_handler.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
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

        template <class Message, class WriteHandler, class MutableBufferSequence>
        auto send(Message const& msg
                , WriteHandler&& handler
                , MutableBufferSequence&& buffers)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            async_write_result_init<WriteHandler> init{
                std::forward<WriteHandler>(handler)
            };

            auto mutable_buffers = detail::make_buffer_sequence_adaptor(buffers);
            msg.encode(mutable_buffers);
            strand_.dispatch(
                    make_async_write_functor(
                          this->shared_from_this()
                        , std::move(init.handler())
                        , std::forward<MutableBufferSequence>(buffers)));
            return init.get();
        }

        template <class Message, class WriteHandler>
        auto send(Message const& msg, WriteHandler&& handler)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            auto buffer = canard::shared_buffer{msg.length()};
            return send(msg
                      , std::forward<WriteHandler>(handler)
                      , std::move(buffer));
        }

        template <class Message>
        auto send(Message const& msg)
            -> typename async_write_result_init<detail::null_handler>::result_type
        {
            return send(msg, detail::null_handler{});
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
