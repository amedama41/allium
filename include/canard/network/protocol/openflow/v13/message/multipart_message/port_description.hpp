#ifndef CANARD_NETWORK_OPENFLOW_V13_PORT_DESCRIPTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_PORT_DESCRIPTION_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/port.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class port_description_request
        : public detail::basic_multipart_request<port_description_request>
    {
    public:
        static ofp_multipart_type const multipart_type_value = OFPMP_PORT_DESC;

        port_description_request()
            : basic_multipart_request{0, 0}
        {
        }
    };

    class port_description_reply
        : public detail::basic_multipart_reply<port_description_reply>
    {
    public:
        using ports_type = std::vector<port>;
        using iterator = ports_type::const_iterator;
        using const_iterator = ports_type::const_iterator;
        using value_type = port;
        using reference = port const&;

        static ofp_multipart_type const multipart_type_value = OFPMP_PORT_DESC;

        port_description_reply(std::vector<port> ports, std::uint16_t const flags)
            : basic_multipart_reply{ports.size() * sizeof(detail::ofp_port), flags}
            , ports_(std::move(ports))
        {
        }

        auto begin() const
            -> const_iterator
        {
            return ports_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return ports_.end();
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> port_description_reply
        {
            auto reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(detail::ofp_multipart_reply)) {
                throw std::runtime_error{(boost::format{"%1%: invalid length(%2%) for buffer length(%3%)"}
                    % __func__ % reply.header.length % std::distance(first, last)).str()};
            }
            if (std::distance(first, last) % sizeof(detail::ofp_port) != 0) {
                throw std::runtime_error{(boost::format{"%1%: invalid buffer length(%2%)"}
                    % __func__ % std::distance(first, last)).str()};
            }

            auto ports = std::vector<port>{};
            ports.reserve(std::distance(first, last) / sizeof(detail::ofp_port));
            while (first != last) {
                ports.push_back(port::decode(first, std::next(first, sizeof(detail::ofp_port))));
            }

            return {reply, std::move(ports)};
        }

    private:
        port_description_reply(detail::ofp_multipart_reply const& reply, std::vector<port> ports)
            : basic_multipart_reply{reply}
            , ports_(std::move(ports))
        {
        }

    private:
        std::vector<port> ports_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_PORT_DESCRIPTION_HPP
// vim: path+=../..
