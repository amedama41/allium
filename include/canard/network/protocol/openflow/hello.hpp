#ifndef CANARD_NETWORK_OPENFLOW_HELLO_HPP
#define CANARD_NETWORK_OPENFLOW_HELLO_HPP

#include <cstdint>
#include <cstring>
#include <boost/asio/buffer.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/message/hello.hpp>
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>

namespace canard {
namespace network {
namespace openflow {

    using hello = v13::hello;
    using ofp_header = v13::v13_detail::ofp_header;
    namespace hello_elements = v13::hello_elements;

    namespace detail {

        auto read_ofp_header(boost::asio::const_buffer const& buffer)
            -> ofp_header
        {
            auto header = ofp_header{};
            std::memcpy(&header, boost::asio::buffer_cast<std::uint8_t const*>(buffer), sizeof(header));
            return v13::v13_detail::ntoh(header);
        }

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_HELLO_HPP
