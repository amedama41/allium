#ifndef CANARD_ASIO_SHARED_BUFFER_HPP
#define CANARD_ASIO_SHARED_BUFFER_HPP

#include <cstddef>
#include <boost/asio/buffer.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_array.hpp>

namespace canard {

    class shared_buffer
    {
    public:
        using value_type = boost::asio::mutable_buffer;

        struct const_iterator
            : boost::iterator_facade<
                const_iterator
              , value_type
              , boost::random_access_traversal_tag
              , value_type
            >
        {
            const_iterator() noexcept
                : ptr()
            {}

            explicit const_iterator(shared_buffer const* const ptr) noexcept
                : ptr(ptr)
            {}

            auto dereference() const
                -> boost::asio::mutable_buffer
            { return boost::asio::buffer(ptr->data_.get(), ptr->size_); }

            auto equal(const_iterator other) const noexcept
                -> bool
            { return ptr == other.ptr; }

            void increment() noexcept
            { ++ptr; }

            void decrement() noexcept
            { --ptr; }

            void advance(difference_type const n) noexcept
            { ptr += n; }

            auto distance_to(const_iterator other) const
                -> difference_type
            { return std::distance(ptr, other.ptr); }

            shared_buffer const* ptr;
        };

        shared_buffer() noexcept
            : data_{}, size_{}
        {
        }

        explicit shared_buffer(std::size_t const buffer_size)
            : data_{new unsigned char[buffer_size]}
            , size_{buffer_size}
        {
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return const_iterator{this};
        }

        auto end() const noexcept
            -> const_iterator
        {
            return const_iterator{this + 1};
        }

        auto data() noexcept
            -> unsigned char*
        {
            return data_.get();
        }

        auto size() const noexcept
            -> std::size_t
        {
            return size_;
        }

        void resize(std::size_t const buffer_size)
        {
            data_.reset(new unsigned char[buffer_size]);
            size_ = buffer_size;
        }

    private:
        boost::shared_array<unsigned char> data_;
        std::size_t size_;
    };

} // namespace canard

#endif // CANARD_ASIO_SHARED_BUFFER_HPP
