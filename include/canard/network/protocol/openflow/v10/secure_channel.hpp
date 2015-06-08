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
#include <canard/network/utils/thread_pool.hpp>
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
        secure_channel(Socket socket, utils::thread_pool& thread_pool)
            : stream_{std::move(socket)}
            , strand_{stream_.next_layer().get_io_service()}
            , thread_pool_(thread_pool)
        {
        }

        auto thread_pool()
            -> utils::thread_pool&
        {
            return thread_pool_;
        }

        void close()
        {
            auto channel = this->shared_from_this();
            strand_.post([channel]{
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
            auto init = async_write_result_init<WriteHandler>{
                std::forward<WriteHandler>(handler)
            };

            auto mutable_buffers = detail::make_buffer_sequence_adaptor(buffers);
            msg.encode(mutable_buffers);
            strand_.post(
                    make_write_op(
                          this->shared_from_this(), std::move(init.handler())
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
                    , strand_.wrap(std::forward<WriteHandler>(handler)));
        }

    private:
        template <class WriteHandler, class MutableBufferSequence>
        struct write_op
        {
            void operator()()
            {
                auto const channel = channel_.get();
                auto buffers = std::move(buffers_);
                channel->async_send(std::move(buffers), std::move(*this));
            }

            void operator()(boost::system::error_code const& ec, std::size_t const bytes_transferred)
            {
                channel_->thread_pool().post(
                        canard::detail::bind(std::move(handler_), ec, bytes_transferred));
            }

            std::shared_ptr<secure_channel> channel_;
            WriteHandler handler_;
            MutableBufferSequence buffers_;
        };

        template <class WriteHandler, class MutableBufferSequence>
        static auto make_write_op(
                  std::shared_ptr<secure_channel> c, WriteHandler&& h
                , MutableBufferSequence&& buf)
            -> write_op<
                      canard::remove_cv_and_reference_t<WriteHandler>
                    , canard::remove_cv_and_reference_t<MutableBufferSequence>
               >
        {
            return write_op<
                  canard::remove_cv_and_reference_t<WriteHandler>
                , canard::remove_cv_and_reference_t<MutableBufferSequence>
            >{
                  std::move(c), std::forward<WriteHandler>(h)
                , std::forward<MutableBufferSequence>(buf)
            };
        }

    protected:
        canard::queueing_write_stream<Socket> stream_;
        boost::asio::io_service::strand strand_;
        utils::thread_pool& thread_pool_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_SECURE_CHANNEL_HPP
