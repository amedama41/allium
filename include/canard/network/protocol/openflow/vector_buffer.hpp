#ifndef CANARD_NETWORK_OPENFLOW_VECTOR_BUFFER_HPP
#define CANARD_NETWORK_OPENFLOW_VECTOR_BUFFER_HPP

#include <cstddef>
#include <utility>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

namespace canard {
namespace network {
namespace openflow {

    template <class... Ts>
    void openflow_buffer_reserve(
            std::vector<Ts...>& vec, std::size_t const size)
    {
        vec.reserve(size);
    }

    template <class... Ts, class Range>
    auto openflow_buffer_push_back(std::vector<Ts...>& vec, Range&& range)
        -> std::vector<Ts...>&
    {
        return boost::push_back(vec, std::forward<Range>(range));
    }

    template <class... Ts>
    auto openflow_buffer_to_const_buffers(std::vector<Ts...> const& vec)
        -> boost::asio::const_buffers_1
    {
        return boost::asio::buffer(vec);
    }

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_VECTOR_BUFFER_HPP
