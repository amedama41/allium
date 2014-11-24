#ifndef CANARD_NETWORK_OPENFLOW_HELLO_HPP
#define CANARD_NETWORK_OPENFLOW_HELLO_HPP

#include <cstdint>
#include <cstring>
#include <boost/asio/buffer.hpp>
#include <canard/network/protocol/openflow/v13/message/hello.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {

    using hello = v13::hello;
    using ofp_header = v13::detail::ofp_header;

    namespace detail {

        auto read_ofp_header(boost::asio::const_buffer const& buffer)
            -> ofp_header
        {
            auto header = ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<std::uint8_t const*>(buffer), sizeof(header));
            return v13::detail::ntoh(header);
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_HELLO_HPP
