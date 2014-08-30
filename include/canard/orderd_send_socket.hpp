#ifndef CANARD_ORDER_SEND_SOCKET
#define CANARD_ORDER_SEND_SOCKET

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/handler_type.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/system/error_code.hpp>

namespace canard {

    namespace detail {

        namespace completion_handler_detail {

            template <class Handler>
            class completion_handler
            {
            public:
                completion_handler(Handler handler, std::size_t const writing_size)
                    : handler_(std::move(handler))
                    , writing_size_(writing_size)
                {
                }

                auto operator()(boost::system::error_code const& error
                              , std::size_t const bytes_transferred)
                    -> std::size_t
                {
                    if (bytes_transferred != writing_size_) {
                        invoke(error, bytes_transferred);
                        return bytes_transferred;
                    }

                    invoke(boost::system::error_code{}, writing_size_);
                    return writing_size_;
                }

            private:
                void invoke(boost::system::error_code const& error
                          , std::size_t const bytes_transferred)
                {
                    using boost::asio::asio_handler_invoke;

                    asio_handler_invoke([=]() {
                        handler_(error, bytes_transferred);
                    }, std::addressof(handler_));
                }

                template <class Function>
                friend void asio_handler_invoke(Function&& function, completion_handler* this_handler)
                {
                    std::cout << "debug: " << __func__ << std::endl;
                    using boost::asio::asio_handler_invoke;
                    asio_handler_invoke(std::forward<Function>(function), std::addressof(this_handler->handler_));
                }

            private:
                Handler handler_;
                std::size_t writing_size_;
            };

        } // namespace completion_handler_detail

        using completion_handler_detail::completion_handler;

        template <class Handler, class ConstBufferSequence>
        auto make_completion_handler(Handler&& handler, ConstBufferSequence const& buffers)
            -> completion_handler<typename std::remove_cv<typename std::remove_reference<Handler>::type>::type>
        {
            return completion_handler<typename std::remove_cv<typename std::remove_reference<Handler>::type>::type>{
                std::forward<Handler>(handler), boost::asio::buffer_size(buffers)
            };
        }

    } // namespace detail

    template <class Socket>
    class orderd_send_socket
    {
        template <class Handler>
        using concrete_handler_type = typename boost::asio::handler_type<
                Handler, void(boost::system::error_code, std::size_t)
        >::type;

        template <class Handler>
        using async_result = boost::asio::async_result<concrete_handler_type<Handler>>;

        class ordered_send_op
        {
        public:
            explicit ordered_send_op(orderd_send_socket& socket)
                : socket_{socket}
            {
            }

            void operator()(boost::system::error_code const& error, std::size_t const bytes_transferred)
            {
                invoke_handlers(error, bytes_transferred);
                socket_.sending_handler_.clear();

                if (socket_.waiting_handlers_.empty()) {
                    return;
                }

                socket_.sending_handler_.swap(socket_.waiting_handlers_);
                auto buffers = std::move(socket_.waiting_buffers_);
                boost::asio::async_write(socket_.next_layer(), std::move(buffers), *this);
            }

        private:
            void invoke_handlers(boost::system::error_code const& error, std::size_t bytes_transferred)
            {
                boost::for_each(socket_.sending_handler_, [&](handler_type& handler) {
                    bytes_transferred -= handler(error, bytes_transferred);
                });
            }

        private:
            orderd_send_socket& socket_;
        };

    public:
        using next_layer_type = typename std::remove_reference<Socket>::type;

        using lowest_layer_type = typename next_layer_type::lowest_layer_type;

        using native_handle_type = typename next_layer_type::native_handle_type;


        explicit orderd_send_socket(boost::asio::io_service& io_service)
            : socket_{io_service}
        {
        }

        explicit orderd_send_socket(Socket socket)
            : socket_{std::move(socket)}
        {
        }

        auto get_io_service()
            -> boost::asio::io_service&
        {
            return socket_.get_io_service();
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
        auto async_read_some(MutableBufferSequence&& buffer, ReadHandler handler)
            -> typename async_result<ReadHandler>::type
        {
            return socket_.async_read_some(std::forward<MutableBufferSequence>(buffer), std::move(handler));
        }

        template <class ConstBufferSequence, class WriteHandler>
        auto async_write_some(ConstBufferSequence&& buffers, WriteHandler handler)
            -> typename async_result<WriteHandler>::type
        {
            auto concrete_handler = concrete_handler_type<WriteHandler>(std::move(handler));
            auto result = async_result<WriteHandler>{concrete_handler};

            if (sending_handler_.empty()) {
                sending_handler_.emplace_back(
                        detail::make_completion_handler(std::move(concrete_handler), buffers));
                boost::asio::async_write(next_layer(), buffers, ordered_send_op{*this});
            }
            else {
                waiting_handlers_.emplace_back(
                        detail::make_completion_handler(std::move(concrete_handler), buffers));
                boost::push_back(waiting_buffers_, std::forward<ConstBufferSequence>(buffers));
            }
            return result.get();
        }

        void close()
        {
            next_layer().close();
        }

    private:
        using handler_type = std::function<
            std::size_t(boost::system::error_code const&, std::size_t const)
        >;

        Socket socket_;
        std::vector<boost::asio::const_buffer> waiting_buffers_;
        std::vector<handler_type> waiting_handlers_;
        std::vector<handler_type> sending_handler_;
    };

} // namespace canard

#endif // CANARD_ORDER_SEND_SOCKET
