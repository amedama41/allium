#ifndef CANARD_ASIO_SHARED_BUFFER_HPP
#define CANARD_ASIO_SHARED_BUFFER_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <boost/asio/buffer.hpp>

namespace canard {

    class shared_buffer
    {
    public:
        using value_type = boost::asio::mutable_buffer;
        using const_iterator = value_type const*;

        shared_buffer()
            : data_(std::make_shared<std::vector<std::uint8_t>>())
            , buffer_(boost::asio::buffer(*data_))
        {
        }

        explicit shared_buffer(std::size_t const buffer_size)
            : data_(std::make_shared<std::vector<std::uint8_t>>(buffer_size))
            , buffer_(boost::asio::buffer(*data_))
        {
        }

        auto begin() const
            -> const_iterator
        {
            return std::addressof(buffer_);
        }

        auto end() const
            -> const_iterator
        {
            return std::addressof(buffer_) + 1;
        }

        void resize(std::size_t const buffer_size)
        {
            data_->resize(buffer_size);
            buffer_ = boost::asio::buffer(*data_);
        }

    private:
        std::shared_ptr<std::vector<std::uint8_t>> data_;
        boost::asio::mutable_buffer buffer_;
    };

} // namespace canard

#endif // CANARD_ASIO_SHARED_BUFFER_HPP
