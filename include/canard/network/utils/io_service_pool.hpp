#ifndef CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP
#define CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP

#include <cstddef>
#include <atomic>
#include <future>
#include <memory>
#include <vector>
#include <boost/asio/io_service.hpp>

namespace canard {
namespace network {
namespace utils {

    class io_service_pool
    {
    public:
        explicit io_service_pool(
                std::size_t const nio_services,
                std::size_t const nthreads_per_io_srv = 1)
            : index_{0}
            , nthreads_per_io_srv_{nthreads_per_io_srv}
        {
            io_services_.reserve(nio_services);
            works_.reserve(nio_services);

            for (auto i = std::size_t{0}; i < nio_services; ++i) {
                io_services_.push_back(
                        std::unique_ptr<boost::asio::io_service>{
                            new boost::asio::io_service{nthreads_per_io_srv}
                        });
                works_.emplace_back(*io_services_[i]);
            }
        }

        ~io_service_pool()
        {
            stop();
        }

        auto get_io_service()
            -> boost::asio::io_service&
        {
            auto const next_index
                = index_.fetch_add(1, std::memory_order_relaxed);
            return *io_services_[next_index % io_services_.size()];
        }

        auto io_service_count() const noexcept
            -> std::size_t
        {
            return io_services_.size();
        }

        auto thread_count() const noexcept
            -> std::size_t
        {
            return io_services_.size() * nthreads_per_io_srv_;
        }

        void stop()
        {
            for (auto&& io_srv : io_services_) {
                try {
                    io_srv->stop();
                }
                catch (std::exception const&) {
                }
            }
            for (auto&& fut : futures_) {
                if (fut.valid()) {
                    fut.wait();
                }
            }
            futures_.clear();
        }

        void reset()
        {
            for (auto&& io_srv : io_services_) {
                io_srv->reset();
            }
        }

        void run()
        {
            if (!futures_.empty()) {
                return; // already running
            }

            futures_.reserve(thread_count());
            for (auto&& io_srv : io_services_) {
                for (auto i = std::size_t{0}; i < nthreads_per_io_srv_; ++i) {
                    futures_.push_back(std::async(
                                  std::launch::async
                                , [&io_srv]{ return io_srv->run(); }));
                }
            }
        }

    private:
        std::vector<std::unique_ptr<boost::asio::io_service>> io_services_;
        std::atomic<std::size_t> index_;
        std::vector<boost::asio::io_service::work> works_;
        std::size_t nthreads_per_io_srv_;
        std::vector<std::future<std::size_t>> futures_;
    };

} // namespace utils
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP
