#ifndef CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP
#define CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP

#include <cstddef>
#include <utility>
#include <boost/range/algorithm/copy.hpp>
#include <canard/asio/shared_buffer.hpp>

namespace canard {
namespace network {
namespace openflow {

    struct shared_buffer_generator
    {
        shared_buffer_generator() noexcept
            : buffer{}, it{buffer.data()}
        {
        }

        explicit shared_buffer_generator(canard::shared_buffer& buffer)
            : buffer(buffer), it{buffer.data()}
        {
        }

        explicit shared_buffer_generator(canard::shared_buffer&& buffer)
            : buffer(std::move(buffer)), it{buffer.data()}
        {
        }

        template <class Range>
        void push_back(Range&& range)
        {
            it = boost::copy(std::forward<Range>(range), it);
        }

        void reserve(std::size_t const size)
        {
            buffer.resize(size);
            it = buffer.data();
        }

        auto to_const_buffers() const& noexcept
            -> canard::shared_buffer const&
        {
            return buffer;
        }

        auto to_const_buffers() && noexcept
            -> canard::shared_buffer&&
        {
            return std::move(buffer);
        }

        canard::shared_buffer buffer;
        unsigned char* it;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_SHARED_BUFFER_GENERATOR_HPP
