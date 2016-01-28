#ifndef CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP
#define CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP

#include <cstddef>
#include <memory>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/distance.hpp>

namespace canard {
namespace network {
namespace openflow {

    class binary_data
    {
    public:
        binary_data()
            : data_{nullptr}
            , size_{0}
        {
        }

        binary_data(std::unique_ptr<unsigned char[]> data
                  , std::size_t const size) noexcept
            : data_(std::move(data))
            , size_(size)
        {
        }

        template <class Range>
        explicit binary_data(Range const& range)
             : data_{new unsigned char[boost::distance(range)]}
             , size_(boost::distance(range))
        {
            boost::copy(range, data_.get());
        }

        auto begin() const noexcept
            -> unsigned char const*
        {
            return data_.get();
        }

        auto end() const noexcept
            -> unsigned char const*
        {
            return data_.get() + size_;
        }

        auto data() const& noexcept
            -> std::unique_ptr<unsigned char[]> const&
        {
            return data_;
        }

        auto data() && noexcept
            -> std::unique_ptr<unsigned char[]>
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

    private:
        std::unique_ptr<unsigned char[]> data_;
        std::size_t size_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_BINARY_DATA_HPP
