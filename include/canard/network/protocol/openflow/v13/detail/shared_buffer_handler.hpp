#ifndef CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/asio/async_result.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/handler_type.hpp>
#include <canard/asio/detail/bind_handler.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <class Handler>
        class shared_buffer_handler
        {
        public:
            explicit shared_buffer_handler(Handler handler)
                : handler_(std::move(handler))
                , buffer_(std::make_shared<std::vector<std::uint8_t>>())
            {
            }

            template <class... Args>
            void operator()(Args&&... args) const
            {
                auto function = canard::detail::bind(std::move(handler_), std::forward<Args>(args)...);
                using boost::asio::asio_handler_invoke;
                asio_handler_invoke(function, std::addressof(function.handler()));
            }

            auto buffer() const
                -> std::vector<std::uint8_t>&
            {
                return *buffer_;
            }

            auto base_handler()
                -> Handler&
            {
                return handler_;
            }

        private:
            Handler handler_;
            std::shared_ptr<std::vector<std::uint8_t>> buffer_;
        };

        template <class Handler>
        auto make_shared_buffer_handler(Handler&& handler)
            -> shared_buffer_handler<typename canard::remove_cv_and_reference<Handler>::type>
        {
            return shared_buffer_handler<typename canard::remove_cv_and_reference<Handler>::type>{
                std::forward<Handler>(handler)
            };
        }

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

namespace boost {
namespace asio {

    template <class Handler>
    class async_result<canard::network::openflow::v13::v13_detail::shared_buffer_handler<Handler>>
        : public async_result<Handler>
    {
    public:
        using handler_type = canard::network::openflow::v13::v13_detail::shared_buffer_handler<Handler>;
        using type = typename async_result<Handler>::type;

        explicit async_result(handler_type& handler)
            : async_result<Handler>(handler.base_handler())
        {
        }

        auto get()
            -> type
        {
            return async_result<Handler>::get();
        }
    };

} // namespace asio
} // namespace boost

#endif // CANARD_NETWORK_OPENFLOW_V13_SHARED_BUFFER_HANDLER_HPP
