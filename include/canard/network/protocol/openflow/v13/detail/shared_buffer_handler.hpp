#ifndef CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/asio/handler_invoke_hook.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class Handler>
        class shared_buffer_handler
        {
        public:
            explicit shared_buffer_handler(Handler handler)
                : handler_(std::move(handler))
                , buffer_(std::make_shared<std::vector<unsigned char>>())
            {
            }

            template <class... Args>
            void operator()(Args&&... args) const
            {
                auto handler = handler_;
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke([=]() mutable {
                    handler(std::move(args)...);
                }, std::addressof(handler_));
            }

            auto buffer() const
                -> std::vector<unsigned char>&
            {
                return *buffer_;
            }

        private:
            Handler handler_;
            std::shared_ptr<std::vector<unsigned char>> buffer_;
        };

        template <class Handler>
        auto make_shared_buffer_handler(Handler&& handler)
            -> shared_buffer_handler<typename std::remove_reference<Handler>::type>
        {
            using handler_type = typename std::remove_reference<Handler>::type;
            return shared_buffer_handler<handler_type>{std::forward<Handler>(handler)};
        }

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP
