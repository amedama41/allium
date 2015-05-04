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

        template <class Message, class WriteHandler, class Container>
        auto send(Message const& msg, WriteHandler&& handler, Container& buffer)
            -> typename async_write_result_init<WriteHandler>::result_type
        {
            auto init = async_write_result_init<WriteHandler>{
                std::forward<WriteHandler>(handler)
            };

            buffer.reserve(msg.length());
            msg.encode(buffer);
            strand_.dispatch(make_write_op(this->shared_from_this(), std::move(init.handler()), buffer));

            return init.get();
        }

    protected:
        template <class ConstBufferSequence, class WriteHandler>
        void async_send(ConstBufferSequence const& buffers, WriteHandler&& handler)
        {
            return stream_.async_write_some(buffers, strand_.wrap(std::forward<WriteHandler>(handler)));
        }

    private:
        template <class WriteHandler, class Container>
        struct write_op
        {
            void operator()()
            {
                auto const channel = channel_.get();
                channel->async_send(boost::asio::buffer(buffer_), std::move(*this));
            }

            void operator()(boost::system::error_code const& ec, std::size_t const bytes_transferred)
            {
                channel_->thread_pool().post(
                        canard::detail::bind(std::move(handler_), ec, bytes_transferred));
            }

            std::shared_ptr<secure_channel> channel_;
            WriteHandler handler_;
            Container& buffer_;
        };

        template <class WriteHandler, class Container>
        static auto make_write_op(std::shared_ptr<secure_channel> c, WriteHandler&& h, Container& buf)
            -> write_op<canard::remove_cv_and_reference_t<WriteHandler>, Container>
        {
            return write_op<canard::remove_cv_and_reference_t<WriteHandler>, Container>{
                std::move(c), std::forward<WriteHandler>(h), buf
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
