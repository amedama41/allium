#ifndef CANARD_NETWORK_UTILS_THREAD_POOL_HPP
#define CANARD_NETWORK_UTILS_THREAD_POOL_HPP

#include <thread>
#include <utility>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/for_each.hpp>

namespace canard {
namespace net {
namespace utils {

    class thread_pool
    {
        using io_service_ptr = std::shared_ptr<boost::asio::io_service>;

    public:
        explicit thread_pool(std::size_t const threads = 1
                , io_service_ptr io_service = std::make_shared<boost::asio::io_service>())
            : io_service_(std::move(io_service))
            , work_(std::make_shared<boost::asio::io_service::work>(*io_service_))
        {
            for (auto i = std::size_t{0}; i < threads; ++i) {
                threads_.emplace_back([&](){
                    io_service_->run();
                });
            }
        }

    private:
        thread_pool(thread_pool const&) = delete;
        auto operator=(thread_pool const&) -> thread_pool& = delete;

    public:
        thread_pool(thread_pool&&) noexcept = default;
        auto operator=(thread_pool&&) noexcept -> thread_pool& = default;

        ~thread_pool()
        {
            work_.reset();
            boost::for_each(threads_ | boost::adaptors::reversed, [](std::thread& t) {
                t.join();
            });
        }

        auto thread_count() const
            -> std::size_t
        {
            return threads_.size();
        }

        template <class Handler>
        void post(Handler&& handler)
        {
            io_service_->post(std::forward<Handler>(handler));
        }

    private:
        io_service_ptr io_service_;
        std::vector<std::thread> threads_;
        std::shared_ptr<boost::asio::io_service::work> work_;
    };

} // namespace utils
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_UTILS_THREAD_POOL_HPP
