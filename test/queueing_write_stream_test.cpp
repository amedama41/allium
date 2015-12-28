#define BOOST_TEST_DYN_LINK
#include <canard/asio/queueing_write_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <algorithm>
#include <future>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/barrier.hpp>
#include "libs/stub_stream.hpp"

BOOST_AUTO_TEST_SUITE(queueing_write_stream_test)

namespace asio = boost::asio;
namespace sys = boost::system;
using stub_stream = canard_test::stub_stream<char>;

template <std::size_t NumBuffers = 10>
struct buffer_fixture
{
  buffer_fixture()
    : buffers(num_buffers)
  {
    constexpr auto buffer_size = std::size_t{512};
    for (auto i = 0; i < num_buffers; ++i) {
      buffers[i] = std::string(buffer_size, char('0' + i));
    }
  }

  auto joined_buffers() const
    -> std::vector<char>
  {
    return boost::copy_range<std::vector<char>>(
        boost::algorithm::join(buffers, ""));
  }

  enum { num_buffers = NumBuffers, buffer_size = 512 };
  std::vector<std::string> buffers;
};


BOOST_AUTO_TEST_SUITE(default_context)

  using stream_type = canard::queueing_write_stream<stub_stream>;

  BOOST_AUTO_TEST_CASE(construct_from_stream_test)
  {
    asio::io_service io_service{};
    auto max_size = std::size_t{5000};
    auto ec = make_error_code(sys::errc::bad_message);
    auto base_stream = stub_stream{io_service, max_size, ec};

    stream_type stream{std::move(base_stream)};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }


  BOOST_AUTO_TEST_CASE(construct_from_arguments_test)
  {
    asio::io_service io_service{};
    auto max_size = std::size_t{3000};
    auto ec = make_error_code(sys::errc::no_stream_resources);

    stream_type stream{io_service, max_size, ec};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }


  BOOST_FIXTURE_TEST_CASE(continuous_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{io_service};
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    for (auto&& buf : buffers) {
      stream.async_write_some(
            asio::buffer(buf)
          , [&](sys::error_code const& ec, std::size_t const size) {
              result.emplace_back(ec, size);
      });
    }
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }


  BOOST_FIXTURE_TEST_CASE(continuous_short_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{io_service, std::size_t{128}};
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    for (auto&& buf : buffers) {
      stream.async_write_some(
            asio::buffer(buf)
          , [&](sys::error_code const& ec, std::size_t const size) {
              result.emplace_back(ec, size);
      });
    }
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

  BOOST_FIXTURE_TEST_CASE(chained_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{io_service, std::size_t{128}};
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    auto counter = std::size_t{};
    std::function<void(sys::error_code const& ec, std::size_t const size)> func;
    func = [&](sys::error_code const& ec, std::size_t const size) {
      result.emplace_back(ec, size);
      if (++counter != num_buffers) {
        stream.async_write_some(asio::buffer(buffers[counter]), func);
      }
    };
    stream.async_write_some(asio::buffer(buffers[0]), func);
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

BOOST_AUTO_TEST_SUITE_END() // default_context


BOOST_AUTO_TEST_SUITE(io_service_context)

  using stream_type = canard::queueing_write_stream<
    stub_stream, asio::io_service
  >;

#if 0 // TODO: enhance queueing_write_stream interface
  BOOST_AUTO_TEST_CASE(construct_from_stream_test)
  {
    asio::io_service io_service{};
    asio::io_service context{};
    auto max_size = std::size_t{5000};
    auto ec = make_error_code(sys::errc::bad_message);
    auto base_stream = stub_stream{io_service, max_size, ec};

    stream_type stream{std::move(base_stream), context};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }

  BOOST_AUTO_TEST_CASE(construct_from_arguments_test)
  {
    asio::io_service io_service{};
    asio::io_service context{};
    auto max_size = std::size_t{3000};
    auto ec = make_error_code(sys::errc::no_stream_resources);

    stream_type stream{context, io_service, max_size, ec};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }
#endif

BOOST_AUTO_TEST_SUITE_END() // io_service_context

BOOST_AUTO_TEST_SUITE(strand_context)

  using stream_type = canard::queueing_write_stream<
    stub_stream, asio::io_service::strand
  >;

  BOOST_AUTO_TEST_CASE(construct_from_stream_test)
  {
    asio::io_service io_service{};
    auto strand = asio::io_service::strand{io_service};
    auto max_size = std::size_t{5000};
    auto ec = make_error_code(sys::errc::bad_message);
    auto base_stream = stub_stream{io_service, max_size, ec};

    stream_type stream{std::move(base_stream), strand};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }

  BOOST_AUTO_TEST_CASE(construct_from_arguments_test)
  {
    asio::io_service io_service{};
    auto strand = asio::io_service::strand{io_service};
    auto max_size = std::size_t{3000};
    auto ec = make_error_code(sys::errc::no_stream_resources);

    stream_type stream{strand, io_service, max_size, ec};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }

  BOOST_FIXTURE_TEST_CASE(continuous_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{asio::io_service::strand{io_service}, io_service};
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    for (auto&& buf : buffers) {
      stream.async_write_some(
            asio::buffer(buf)
          , [&](sys::error_code const& ec, std::size_t const size) {
              result.emplace_back(ec, size);
      });
    }
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

  BOOST_FIXTURE_TEST_CASE(continuous_short_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{
      asio::io_service::strand{io_service}, io_service, std::size_t{128}
    };
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    for (auto&& buf : buffers) {
      stream.async_write_some(
            asio::buffer(buf)
          , [&](sys::error_code const& ec, std::size_t const size) {
              result.emplace_back(ec, size);
      });
    }
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

  BOOST_FIXTURE_TEST_CASE(chained_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    stream_type stream{
      asio::io_service::strand{io_service}, io_service, std::size_t{128}
    };
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    auto counter = std::size_t{};
    std::function<void(sys::error_code const& ec, std::size_t const size)> func;
    func = [&](sys::error_code const& ec, std::size_t const size) {
      result.emplace_back(ec, size);
      if (++counter != num_buffers) {
        stream.async_write_some(asio::buffer(buffers[counter]), func);
      }
    };
    stream.async_write_some(asio::buffer(buffers[0]), func);
    io_service.run();

    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

BOOST_AUTO_TEST_SUITE_END() // strand_context

BOOST_AUTO_TEST_SUITE(original_context)

  struct original_context
  {
    struct data {
      std::size_t alloc_counter = 0;
      std::size_t dealloc_counter = 0;
      std::size_t invoke_counter = 0;
      std::size_t continuation_counter = 0;
    };
    data* ptr;

    friend auto asio_handler_allocate(std::size_t size, original_context* ctx)
      -> void*
    {
      ++ctx->ptr->alloc_counter;
      return operator new(size);
    }

    friend void asio_handler_deallocate(void* p, std::size_t, original_context* ctx)
    {
      ++ctx->ptr->dealloc_counter;
      operator delete(p);
    }

    template <class Function>
    friend void asio_handler_invoke(Function&& function, original_context* ctx)
    {
      ++ctx->ptr->invoke_counter;
      function();
      ++ctx->ptr->invoke_counter;
    }

    friend bool asio_handler_is_continuation(original_context* ctx)
    {
      ++ctx->ptr->continuation_counter;
      return false;
    }
  };

  using stream_type = canard::queueing_write_stream<
    stub_stream, original_context
  >;

  BOOST_AUTO_TEST_CASE(construct_from_stream_test)
  {
    asio::io_service io_service{};
    auto max_size = std::size_t{5000};
    auto ec = make_error_code(sys::errc::bad_message);
    auto base_stream = stub_stream{io_service, max_size, ec};

    stream_type stream{std::move(base_stream)};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }

  BOOST_AUTO_TEST_CASE(construct_from_arguments_test)
  {
    asio::io_service io_service{};
    auto max_size = std::size_t{3000};
    auto ec = make_error_code(sys::errc::no_stream_resources);

    stream_type stream{io_service, max_size, ec};

    BOOST_TEST(&stream.get_io_service() == &io_service);
    BOOST_TEST(stream.next_layer().max_writable_size_per_write() == max_size);
    BOOST_TEST(stream.next_layer().error_code() == ec);
    BOOST_TEST(stream.next_layer().written_data().size() == 0);
  }

  BOOST_FIXTURE_TEST_CASE(continuous_write_test, buffer_fixture<>)
  {
    asio::io_service io_service{};
    auto data = original_context::data{};
    stream_type stream{original_context{&data}, io_service};
    auto result = std::vector<std::tuple<sys::error_code, std::size_t>>();

    for (auto&& buf : buffers) {
      stream.async_write_some(
            asio::buffer(buf)
          , [&](sys::error_code const& ec, std::size_t const size) {
              result.emplace_back(ec, size);
      });
    }
    io_service.run();

    BOOST_TEST(data.alloc_counter == 2);
    BOOST_TEST(data.dealloc_counter == 2);
    BOOST_TEST(data.invoke_counter == 4);
    BOOST_TEST(data.continuation_counter == 0);
    BOOST_TEST(result.size() == num_buffers);
    for (auto&& e : result) {
      BOOST_TEST(std::get<0>(e) == sys::error_code{});
      BOOST_TEST(std::get<1>(e) == buffer_size);
    }
    BOOST_TEST(stream.next_layer().written_data() == joined_buffers());
  }

BOOST_AUTO_TEST_SUITE_END() // original_context

BOOST_AUTO_TEST_SUITE(multi_thread_test)

  using stream_type = canard::queueing_write_stream<
    stub_stream, asio::io_service::strand
  >;

  constexpr auto num_threads = std::size_t{4};


  BOOST_FIXTURE_TEST_CASE(continuous_write_test, buffer_fixture<79>)
  {
    asio::io_service io_service{};
    auto strand = asio::io_service::strand{io_service};
    stream_type stream{strand, io_service, std::size_t{128}};
    auto result = std::vector<
      std::vector<std::tuple<sys::error_code, std::size_t>>
    >(num_threads);

    auto futures = std::vector<std::future<void>>{};
    boost::barrier barrier(num_threads);
    for (auto i = 0; i < num_threads; ++i) {
      futures.push_back(std::async(std::launch::async, [&, i]{
        io_service.post([&, i]{
          for (auto&& buf : buffers) {
            strand.dispatch([&, i]{
              stream.async_write_some(
                    asio::buffer(buf)
                  , [&, i](sys::error_code const& ec, std::size_t const size) {
                      result[i].emplace_back(ec, size);
              });
            });
          }
        });
        barrier.wait();
        io_service.run();
      }));
    }
    for (auto&& e : futures) e.get();

    for (auto i = 0; i < num_threads; ++i) {
      BOOST_TEST(result[i].size() == num_buffers);
      for (auto&& e : result[i]) {
        BOOST_TEST(std::get<0>(e) == sys::error_code{});
        BOOST_TEST(std::get<1>(e) == buffer_size);
      }
    }
    auto const& data = stream.next_layer().written_data();
    BOOST_TEST(data.size() % buffer_size == 0);
    for (auto i = std::size_t{}; i < data.size(); i += buffer_size) {
      BOOST_TEST(std::count(&data[i], &data[i] + buffer_size, data[i]) == buffer_size);
    }
  }


  BOOST_FIXTURE_TEST_CASE(chained_write_test, buffer_fixture<79>)
  {
    asio::io_service io_service{};
    auto strand = asio::io_service::strand{io_service};
    stream_type stream{strand, io_service, std::size_t{128}};
    auto result = std::vector<
      std::vector<std::tuple<sys::error_code, std::size_t>>
    >(num_threads);

    auto counter = std::vector<std::size_t>(num_threads, 0);
    auto func = std::vector<
      std::function<void(sys::error_code const& ec, std::size_t const size)>
    >(num_threads);
    auto futures = std::vector<std::future<void>>{};
    boost::barrier barrier(num_threads);
    for (auto i = 0; i < num_threads; ++i) {
      func[i] = [&, i](sys::error_code const& ec, std::size_t const size) {
        result[i].emplace_back(ec, size);
        if (++counter[i] != num_buffers) {
          stream.async_write_some(asio::buffer(buffers[counter[i]]), strand.wrap(func[i]));
        }
      };
      futures.push_back(std::async(std::launch::async, [&, i]{
        io_service.post(strand.wrap([&, i]{
          stream.async_write_some(asio::buffer(buffers[0]), strand.wrap(func[i]));
        }));
        barrier.wait();
        io_service.run();
      }));
    }
    for (auto&& e : futures) e.get();

    for (auto i = 0; i < num_threads; ++i) {
      BOOST_TEST(result[i].size() == num_buffers);
      for (auto&& e : result[i]) {
        BOOST_TEST(std::get<0>(e) == sys::error_code{});
        BOOST_TEST(std::get<1>(e) == buffer_size);
      }
    }
    auto const& data = stream.next_layer().written_data();
    BOOST_TEST(data.size() % buffer_size == 0);
    for (auto i = std::size_t{}; i < data.size(); i += buffer_size) {
      BOOST_TEST(std::count(&data[i], &data[i] + buffer_size, data[i]) == buffer_size);
    }
  }

BOOST_AUTO_TEST_SUITE_END() // multi_thread_test

BOOST_AUTO_TEST_SUITE_END() // queueing_write_stream_test

// vim: set et ts=2 sw=2 sts=0 :
