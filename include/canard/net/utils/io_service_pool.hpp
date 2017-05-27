#ifndef CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP
#define CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP

#include <cstddef>
#include <algorithm>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <vector>
#include <boost/asio/io_service.hpp>

namespace canard {
namespace net {
namespace utils {

  class io_service_pool
  {
  public:
    explicit io_service_pool(
          std::size_t const nio_services
        , std::size_t const nthreads_per_io_srv = 1)
      : index_{0}
      , nthreads_per_io_srv_{nthreads_per_io_srv}
    {
      io_services_.reserve(nio_services);
      futures_.reserve(thread_count());
      for (auto i = std::size_t{0}; i < nio_services; ++i) {
        io_services_.push_back(
            std::unique_ptr<boost::asio::io_service>{
              new boost::asio::io_service{nthreads_per_io_srv}
            });
      }
    }

    ~io_service_pool()
    {
      stop();
    }

    auto get_io_service()
      -> boost::asio::io_service&
    {
      auto const next_index = index_.fetch_add(1, std::memory_order_relaxed);
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

    template <class Func>
    void start(bool const block, Func&& func)
    {
      std::lock_guard<std::mutex> lock{mutex_};
      if (!futures_.empty()) {
        return; // already running or not reset yet
      }

      for (auto i = std::size_t(block ? 1 : 0); i < thread_count(); ++i) {
        auto const id = i / nthreads_per_io_srv_;
        futures_.push_back(
            std::async(
                std::launch::async
              , func, std::ref(*io_services_[id]), id, i));
      }
      if (block) {
        func(*io_services_[0], 0, 0);
      }
    }

    void run(bool const block)
    {
      start(block
          , [](boost::asio::io_service& io_service, std::size_t, std::size_t) {
          io_service.run();
      });
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
    }

    void reset()
    {
      wait_for_all_threads_to_stop_completely();
      for (auto&& io_srv : io_services_) {
        io_srv->reset();
      }
    }

    class work
    {
    public:
      explicit work(io_service_pool& pool)
        : pool_{pool}
      {
        works_.reserve(pool_.io_services_.size());
        for (auto&& io_srv : pool_.io_services_) {
          works_.emplace_back(*io_srv);
        }
      }

      work(work const&) = default;
      auto operator=(work const&) -> work& = delete;

      auto get_io_service_pool()
        -> io_service_pool&
      {
        return pool_;
      }

    private:
      io_service_pool& pool_;
      std::vector<boost::asio::io_service::work> works_;
    };

  private:
    void wait_for_all_threads_to_stop_completely()
    {
      std::lock_guard<std::mutex> lock{mutex_};
      if (futures_.empty()) {
        return;
      }
      for (auto&& fut : futures_) {
        fut.wait();
      }
      futures_.clear();
    }

  private:
    std::vector<std::unique_ptr<boost::asio::io_service>> io_services_;
    std::atomic<std::size_t> index_;
    std::size_t nthreads_per_io_srv_;
    std::vector<std::future<void>> futures_;
    std::mutex mutex_;
  };

} // namespace utils
} // namespace net
} // namespace canard

#endif // CANARD_NETWORK_UTILS_IO_SERVICE_POOL_HPP
