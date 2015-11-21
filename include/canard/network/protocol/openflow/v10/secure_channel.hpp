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
                  Socket& socket
                , boost::asio::io_service::strand strand)
            : stream_{std::move(socket), std::move(strand)}
        {
        }

        explicit secure_channel(Socket socket)
            : secure_channel{
                  socket
                , boost::asio::io_service::strand{socket.get_io_service()}
            }
        {
        }

        void close()
        {
            auto channel = this->shared_from_this();
            stream_.invoke([channel]{
                if (channel->stream_.lowest_layer().is_open()) {
                    auto ignore = boost::system::error_code{};
                    channel->stream_.lowest_layer().close(ignore);
                }
            });
        }

        template <class Message, class WriteHandler, class MutableBufferSequence>
        auto send(Message const& msg, WriteHandler&& handler, MutableBufferSequence buffers)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            async_write_result_init<WriteHandler> init{
                std::forward<WriteHandler>(handler)
            };

            auto mutable_buffers = detail::make_buffer_sequence_adaptor(buffers);
            msg.encode(mutable_buffers);
            stream_.invoke(
                    make_send_func_in_channel_thread(
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
            return send(msg, std::forward<WriteHandler>(handler), std::move(buffer));
        }

        template <class Message>
        auto send(Message const& msg)
            -> typename async_write_result_init<detail::null_handler>::result_type
        {
            return send(msg, detail::null_handler{});
        }

    protected:
        template <class ConstBufferSequence, class WriteHandler>
        void async_send(ConstBufferSequence&& buffers, WriteHandler&& handler)
        {
            return stream_.async_write_some(
                      std::forward<ConstBufferSequence>(buffers)
                    , std::forward<WriteHandler>(handler));
        }

    private:
        template <class WriteHandler, class ConstBufferSequence>
        struct send_in_channel_thread
        {
            void operator()()
            {
                auto const channel = channel_.get();
                channel->async_send(
                          std::move(buffers_)
                        , std::move(handler_));
            }

            std::shared_ptr<secure_channel> channel_;
            WriteHandler handler_;
            ConstBufferSequence buffers_;
        };

        template <class WriteHandler, class ConstBufferSequence>
        static auto make_send_func_in_channel_thread(
                  std::shared_ptr<secure_channel> c
                , WriteHandler&& h
                , ConstBufferSequence&& buf)
            -> send_in_channel_thread<
                      canard::remove_cv_and_reference_t<WriteHandler>
                    , canard::remove_cv_and_reference_t<ConstBufferSequence>
               >
        {
            return send_in_channel_thread<
                  canard::remove_cv_and_reference_t<WriteHandler>
                , canard::remove_cv_and_reference_t<ConstBufferSequence>
            >{
                  std::move(c), std::forward<WriteHandler>(h)
                , std::forward<ConstBufferSequence>(buf)
            };
        }

    protected:
        canard::queueing_write_stream<
            Socket, boost::asio::io_service::strand
        > stream_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
