#ifndef CANARD_NETWORK_OPENFLOW_V13_REQUEST_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_REQUEST_HANDLER_HPP

#include <cstddef>
#include <memory>
#include <utility>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/system/error_code.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/reply_message.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class Handler, class T>
        struct request_handler
        {
        public:
            request_handler(Handler handler, std::shared_ptr<reply_message<T>> reply_impl)
                : handler_{std::move(handler)}
                , reply_{std::move(reply_impl)}
            {
            }

            void operator()(boost::system::error_code const& error, std::size_t)
            {
                using boost::asio::asio_handler_invoke;
                auto h = handler_;
                auto r = std::move(reply_);
                asio_handler_invoke([=]() mutable {
                    h(error, std::move(r));
                }, std::addressof(handler_));
            }

        private:
            Handler handler_;
            std::shared_ptr<reply_message<T>> reply_;
        };

        template <class Handler, class T>
        auto make_request_handler(Handler handler, std::shared_ptr<reply_message<T>> reply)
            -> request_handler<Handler, T>
        {
            return request_handler<Handler, T>{std::move(handler), std::move(reply)};
        }

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_REQUEST_HANDLER_HPP
