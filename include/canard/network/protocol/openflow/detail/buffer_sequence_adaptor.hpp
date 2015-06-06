#ifndef CANARD_NETWORK_OPENFLOW_BUFFER_SEQUENCE_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_BUFFER_SEQUENCE_ADAPTOR_HPP

#include <cstring>
#include <memory>
#include <boost/asio/buffer.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    template <class BufferSequence>
    class buffer_sequence_adaptor
    {
    public:
        using const_iterator = typename BufferSequence::const_iterator;

        explicit buffer_sequence_adaptor(BufferSequence const& buffers)
            : it_(buffers.begin())
            , it_end_(buffers.end())
            , current_buffer_(
                    it_ != it_end_
                    ? boost::asio::mutable_buffer(*it_)
                    : boost::asio::mutable_buffer{})
        {
        }

        template <class T>
        auto push_back(T const& t, std::size_t const remain_size = sizeof(T))
            -> buffer_sequence_adaptor&
        {
            push_back(reinterpret_cast<unsigned char const*>(std::addressof(t))
                    , remain_size);
            return *this;
        }

        auto push_back(unsigned char const* source_ptr, std::size_t remain_size)
            -> buffer_sequence_adaptor&
        {
            while (it_ != it_end_) {
                auto const buffer_size = boost::asio::buffer_size(current_buffer_);
                auto const dest_ptr
                    = boost::asio::buffer_cast<unsigned char*>(current_buffer_);

                if (buffer_size >= remain_size) {
                    std::memcpy(dest_ptr, source_ptr, remain_size);
                    current_buffer_ = current_buffer_ + remain_size;
                    return *this;
                }

                std::memcpy(dest_ptr, source_ptr, remain_size);
                source_ptr += buffer_size;
                remain_size -= buffer_size;
                if (++it_ == it_end_) {
                    break;
                }
                current_buffer_ = *it_;
            }
            throw std::runtime_error{"buffer size is smaller than data object"};
        }

    private:
        const_iterator it_;
        const_iterator it_end_;
        boost::asio::mutable_buffer current_buffer_;
    };

    template <class MutableBufferSequence>
    auto make_buffer_sequence_adaptor(MutableBufferSequence const& buffers)
        -> buffer_sequence_adaptor<
                canard::remove_cv_and_reference_t<MutableBufferSequence>
           >
    {
        return buffer_sequence_adaptor<
            canard::remove_cv_and_reference_t<MutableBufferSequence>
        >{buffers};
    }

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_BUFFER_SEQUENCE_ADAPTOR_HPP
