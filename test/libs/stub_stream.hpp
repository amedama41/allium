#ifndef CANARD_ASIO_STUB_STREAM_HPP
#define CANARD_ASIO_STUB_STREAM_HPP

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>
#include <type_traits>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/handler_type.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>

namespace canard_test {

    template <class CharT>
    class stub_stream
    {
    public:
        using lowest_layer_type = stub_stream;

        template <class T>
        using remove_cv_ref_t = typename std::remove_cv<
            typename std::remove_reference<T>::type
        >::type;

        explicit stub_stream(
                  boost::asio::io_service& io_service
                , std::size_t const max_writable_size_per_write
                        = std::numeric_limits<std::size_t>::max()
                , boost::system::error_code const& ec
                        = boost::system::error_code{})
            : io_service_(io_service)
            , max_writable_size_per_write_(max_writable_size_per_write)
            , ec_(ec)
        {
        }

        auto get_io_service() noexcept
            -> boost::asio::io_service&
        {
            return io_service_;
        }

        auto lowest_layer() noexcept
            -> lowest_layer_type&
        {
            return *this;
        }

        template <
              class ConstBufferSequence, class CompletionToken
            , class WriteHandler = typename boost::asio::handler_type<
                  remove_cv_ref_t<CompletionToken>
                , void(boost::system::error_code, std::size_t)
              >::type
        >
        auto async_write_some(
                ConstBufferSequence const& buffers, CompletionToken&& token)
            -> typename boost::asio::async_result<WriteHandler>::type
        {

            auto handler = WriteHandler(std::forward<CompletionToken>(token));
            boost::asio::async_result<WriteHandler> result{handler};

            auto bytes_transferred = std::size_t{0};
            if (!ec_) {
                auto tmp_written_data = std::vector<CharT>{};
                for (auto&& buffer : buffers) {
                    auto const size = boost::asio::buffer_size(buffer);
                    auto const data
                        = boost::asio::buffer_cast<CharT const*>(buffer);
                    if (size + bytes_transferred < max_writable_size_per_write_) {
                        tmp_written_data.insert(
                                tmp_written_data.end(), data, data + size);
                        bytes_transferred += size;
                    }
                    else {
                        auto const remain_size
                            = max_writable_size_per_write_ - bytes_transferred;
                        tmp_written_data.insert(
                                tmp_written_data.end(), data, data + remain_size);
                        bytes_transferred += remain_size;
                    }
                }
                written_data_.reserve(
                        written_data_.size() + tmp_written_data.size());
                written_data_.insert(
                          written_data_.end()
                        , tmp_written_data.begin(), tmp_written_data.end());
            }

            io_service_.post(canard::detail::bind(
                        std::move(handler), ec_, bytes_transferred));

            return result.get();
        }

        auto max_writable_size_per_write() const noexcept
            -> std::size_t
        {
            return max_writable_size_per_write_;
        }

        void max_writable_size_per_write(
                std::size_t const max_writable_size_per_write) noexcept
        {
            max_writable_size_per_write_ = max_writable_size_per_write;
        }

        auto error_code() const noexcept
            -> boost::system::error_code const&
        {
            return ec_;
        }

        void error_code(boost::system::error_code const& ec)
        {
            ec_ = ec;
        }

        auto written_data() const noexcept
            -> std::vector<CharT> const&
        {
            return written_data_;
        }

    private:
        boost::asio::io_service& io_service_;
        std::size_t max_writable_size_per_write_;
        boost::system::error_code ec_;
        std::vector<CharT> written_data_;
    };

} // namespace canard_test

#endif // CANARD_ASIO_STUB_STREAM_HPP
