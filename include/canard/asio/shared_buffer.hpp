#ifndef CANARD_ASIO_SHARED_BUFFER_HPP
#define CANARD_ASIO_SHARED_BUFFER_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace canard {

    class shared_buffer
    {
        using underlying_buffer_type = std::vector<unsigned char>;

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
            const_iterator() : ptr() {}
            explicit const_iterator(underlying_buffer_type* const ptr)
                : ptr(ptr)
            { }

            auto dereference() const
                -> boost::asio::mutable_buffer
            { return boost::asio::buffer(*ptr); }

            auto equal(const_iterator other) const
                -> bool
            { return ptr == other.ptr; }

            void increment()
            { ++ptr; }

            void decrement()
            { --ptr; }

            void advance(difference_type const n)
            { ptr += n; }

            auto distance_to(const_iterator other) const
                -> difference_type
            { return std::distance(ptr, other.ptr); }

            underlying_buffer_type* ptr;
        };

        shared_buffer()
            : data_(std::make_shared<underlying_buffer_type>())
        {
        }

        explicit shared_buffer(std::size_t const buffer_size)
            : data_(std::make_shared<underlying_buffer_type>(buffer_size))
        {
        }

        auto begin() const
            -> const_iterator
        {
            return const_iterator{data_.get()};
        }

        auto end() const
            -> const_iterator
        {
            return const_iterator{data_.get() + 1};
        }

        void resize(std::size_t const buffer_size)
        {
            data_->resize(buffer_size);
        }

    private:
        std::shared_ptr<underlying_buffer_type> data_;
    };

} // namespace canard

#endif // CANARD_ASIO_SHARED_BUFFER_HPP
