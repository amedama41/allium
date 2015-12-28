#ifndef CANARD_ASIO_CONSUMING_BUFFERS_HPP
#define CANARD_ASIO_CONSUMING_BUFFERS_HPP

#include <cstddef>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

namespace canard {
namespace detail {

    template <class BufferSequence>
    class consuming_buffers
    {
        using const_iterator
            = typename BufferSequence::const_iterator;
        using value_type
            = typename BufferSequence::value_type;
    public:
        explicit consuming_buffers(BufferSequence const& buffers)
            : buffers_(buffers)
            , begin_remainder_(buffers_.begin())
        {
        }

        explicit consuming_buffers(BufferSequence&& buffers)
            : buffers_(std::move(buffers))
            , begin_remainder_(buffers_.begin())
        {
        }

        auto consume(std::size_t& total_bytes_transferred, std::size_t& size)
            -> boost::asio::const_buffer
        {
            for ( ; begin_remainder_ != buffers_.end(); ++begin_remainder_) {
                auto const buffer_size
                    = boost::asio::buffer_size(*begin_remainder_);
                if (buffer_size > size) {
                    auto const buffer = *(begin_remainder_++) + size;
                    total_bytes_transferred += size;
                    size = 0;
                    return buffer;
                }
                total_bytes_transferred += buffer_size;
                size -= buffer_size;
            }
            return boost::asio::const_buffer{};
        }

        template <class Container>
        void push_back_to(Container& container) const
        {
            boost::push_back(
                    container, boost::make_iterator_range(
                        begin_remainder_, buffers_.end()));
        }

    private:
        BufferSequence buffers_;
        const_iterator begin_remainder_;
    };

} // namespace detail
} // namespace canard

#endif // CANARD_ASIO_CONSUMING_BUFFERS_HPP
