#ifndef CANARD_ASIO_CONSUMING_BUFFERS_HPP
#define CANARD_ASIO_CONSUMING_BUFFERS_HPP

#include <utility>
#include <deque>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/range/algorithm/find_if.hpp>

namespace canard {
namespace detail {

    template <class Buffer>
    class consuming_buffers
    {
    public:
        using value_type = Buffer;
        using iterator = typename std::deque<Buffer>::iterator;
        using const_iterator = typename std::deque<Buffer>::const_iterator;

        consuming_buffers() = default;

        template <class BufferSequence>
        explicit consuming_buffers(BufferSequence&& buffers)
            : buffers_(boost::asio::buffers_begin(buffers), boost::asio::buffers_end(buffers))
        {
        }

        auto consume(std::size_t const bytes_transferred)
            -> std::size_t
        {
            auto bytes_consumed = std::size_t{0};
            auto const it = boost::find_if(buffers_, [&](value_type& buffer) {
                auto const buffer_size = boost::asio::buffer_size(buffer);
                if (bytes_transferred - bytes_consumed < buffer_size) {
                    return true;
                }
                bytes_consumed += buffer_size;
                return false;
            });

            if (it != buffers_.end()) {
                *it = *it + (bytes_transferred - bytes_consumed);
                bytes_consumed = bytes_transferred;
            }
            buffers_.erase(buffers_.begin(), it);

            return bytes_consumed;
        }

        void clear()
        {
            buffers_.clear();
        }

        auto empty() const
            -> bool
        {
            return buffers_.empty();
        }

        auto begin()
            -> iterator
        {
            return buffers_.begin();
        }

        auto begin() const
            -> const_iterator
        {
            return buffers_.begin();
        }

        auto end()
            -> iterator
        {
            return buffers_.end();
        }

        auto end() const
            -> const_iterator
        {
            return buffers_.end();
        }

        template <class... Args>
        auto insert(Args&&... args)
            -> decltype(static_cast<std::deque<Buffer>*>(nullptr)->insert(std::forward<Args>(args)...))
        {
            return buffers_.insert(std::forward<Args>(args)...);
        }

    private:
        std::deque<Buffer> buffers_;
    };

} // namespace detail
} // namespace canard

#endif // CANARD_ASIO_CONSUMING_BUFFERS_HPP
