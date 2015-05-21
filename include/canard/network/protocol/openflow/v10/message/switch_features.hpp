#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_FEATURES_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_FEATURES_HPP

#include <cstdint>
#include <stdexcept> // TODO
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/port.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class features_request
        : public v10_detail::basic_openflow_message<features_request>
    {
    public:
        static ofp_type const message_type = OFPT_FEATURES_REQUEST;

        explicit features_request(std::uint32_t const xid = get_xid())
            : header_{OFP_VERSION, message_type, sizeof(header_), xid}
        {
        }

        auto header() const
            -> v10_detail::ofp_header
        {
            return header_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> features_request
        {
            auto const header = detail::decode<v10_detail::ofp_header>(first, last);
            return features_request{header};
        }

    private:
        explicit features_request(v10_detail::ofp_header const header)
            : header_(header)
        {
            if (version() != OFP_VERSION) {
                throw std::runtime_error("invalid version");
            }
            if (type() != message_type) {
                throw std::runtime_error("invalid message type");
            }
        }

    private:
        v10_detail::ofp_header header_;
    };

    class features_reply
        : public v10_detail::basic_openflow_message<features_reply>
    {
    public:
        static ofp_type const message_type = OFPT_FEATURES_REPLY;

        using iterator = std::vector<v10::port>::const_iterator;
        using const_iterator = std::vector<v10::port>::const_iterator;

        auto header() const
            -> v10_detail::ofp_header
        {
            return features_.header;
        }

        auto datapath_id() const
            -> std::uint64_t
        {
            return features_.datapath_id;
        }

        auto n_buffers() const
            -> std::uint32_t
        {
            return features_.n_buffers;
        }

        auto n_tables() const
            -> std::uint8_t
        {
            return features_.n_tables;
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
            -> features_reply
        {
            auto const features = detail::decode<v10_detail::ofp_switch_features>(first, last);
            auto ports = std::vector<port>{};
            ports.reserve((features.header.length - sizeof(features)) / sizeof(v10_detail::ofp_phy_port));
            while (first != last) {
                ports.emplace_back(port::decode(first, last));
            }
            return features_reply{features, std::move(ports)};
        }

    private:
        features_reply(v10_detail::ofp_switch_features const& features, std::vector<port> ports)
            : features_(features)
            , ports_(std::move(ports))
        {
            if (version() != OFP_VERSION) {
                throw std::runtime_error("invalid version");
            }
            if (type() != message_type) {
                throw std::runtime_error("invalid message type");
            }
            if (length() != sizeof(v10_detail::ofp_switch_features) + ports_.size() * sizeof(v10_detail::ofp_phy_port)) {
                throw std::runtime_error("invalid length");
            }
        }

    private:
        v10_detail::ofp_switch_features features_;
        std::vector<v10::port> ports_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_FEATURES_HPP
