#ifndef CANARD_ASIO_CONSUMING_BUFFERS_HPP
#define CANARD_ASIO_CONSUMING_BUFFERS_HPP

#include <cstddef>
#include <iterator>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/numeric.hpp>

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
            , consumed_size_{0}
        {
        }

        explicit consuming_buffers(BufferSequence&& buffers)
            : buffers_(std::move(buffers))
            , begin_remainder_(buffers_.begin())
            , consumed_size_{0}
        {
        }

        auto consume(std::size_t& size)
            -> bool
        {
            for ( ; begin_remainder_ != buffers_.end(); ++begin_remainder_) {
                auto const buffer_size
                    = boost::asio::buffer_size(*begin_remainder_);
                if (buffer_size > consumed_size_ + size) {
                    consumed_size_ += size;
                    size = 0;
                    return false;
                }
                size -= buffer_size - consumed_size_;
                consumed_size_ = 0;
            }
            return true;
        }

        auto total_consumed_size() const
            -> std::size_t
        {
            using boost::adaptors::transformed;
            return boost::accumulate(
                      boost::make_iterator_range(buffers_.begin(), begin_remainder_)
                    | transformed([](value_type const& buf) {
                        return boost::asio::buffer_size(buf);
                      })
                    , consumed_size_);
        }

        template <class Container>
        void push_back_to(Container& container) const
        {
            if (begin_remainder_ == buffers_.end()) {
                return;
            }
            container.push_back(*begin_remainder_ + consumed_size_);
            auto it = begin_remainder_;
            boost::push_back(
                      container
                    , boost::make_iterator_range(++it, buffers_.end()));
        }

    private:
        BufferSequence buffers_;
        const_iterator begin_remainder_;
        std::size_t consumed_size_;
    };

} // namespace detail
} // namespace canard

#endif // CANARD_ASIO_CONSUMING_BUFFERS_HPP
