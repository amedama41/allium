#ifndef CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP
#define CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP

#include <cstddef>
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <boost/range/distance.hpp>

namespace canard {
namespace network {
namespace openflow {

    class binary_data
    {
    public:
        using iterator = unsigned char const*;
        using const_iterator = unsigned char const*;

        using pointer_type = std::unique_ptr<unsigned char[]>;

        binary_data() noexcept
            : data_{nullptr}
            , size_{0}
        {
        }

        binary_data(pointer_type data, std::size_t const size) noexcept
            : data_(std::move(data))
            , size_(size)
        {
        }

        template <class Range>
        explicit binary_data(Range const& range)
             : data_(copy_data(range))
             , size_(boost::distance(range))
        {
        }

        binary_data(binary_data&& other) noexcept
            : data_(std::move(other.data_))
            , size_(other.size_)
        {
            other.size_ = 0;
        }

        auto operator=(binary_data&& other) noexcept
            -> binary_data&
        {
            auto tmp = std::move(other);
            data_.swap(tmp.data_);
            size_ = tmp.size_;
            return *this;
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return data_.get();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return data_.get() + size_;
        }

        auto data() const& noexcept
            -> pointer_type const&
        {
            return data_;
        }

        auto data() && noexcept
            -> pointer_type
        {
            size_ = 0;
            auto data = std::move(data_);
            return data;
        }

        auto size() const noexcept
            -> std::size_t
        {
            return size_;
        }

        template <class Range>
        static auto copy_data(Range const& range)
            -> pointer_type
        {
            using std::begin;
            return copy_data(begin(range), boost::distance(range));
        }

        template <class Iterator>
        static auto copy_data(Iterator first, Iterator last)
            -> typename std::enable_if<
                   !std::is_integral<Iterator>::value, pointer_type
               >::type
        {
            return copy_data(first, std::distance(first, last));
        }

        template <class Iterator, class DistanceType>
        static auto copy_data(Iterator first, DistanceType size)
            -> typename std::enable_if<
                   std::is_integral<DistanceType>::value, pointer_type
               >::type
        {
            if (size) {
                auto copy = pointer_type{new unsigned char[size]};
                std::copy_n(first, size, copy.get());
                return copy;
            }
            return pointer_type{nullptr};
        }

    private:
        pointer_type data_;
        std::size_t size_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP
