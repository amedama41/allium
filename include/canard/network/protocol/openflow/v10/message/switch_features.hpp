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
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FEATURES_REQUEST;

        explicit features_request(std::uint32_t const xid = get_xid()) noexcept
            : header_{
                  protocol::OFP_VERSION
                , message_type
                , sizeof(v10_detail::ofp_header)
                , xid
              }
        {
        }

        auto header() const noexcept
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
            auto const header
                = detail::decode<v10_detail::ofp_header>(first, last);
            return features_request{header};
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length != sizeof(v10_detail::ofp_header)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit features_request(v10_detail::ofp_header const& header)
            : header_(header)
        {
        }

    private:
        v10_detail::ofp_header header_;
    };

    class features_reply
        : public v10_detail::basic_openflow_message<features_reply>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FEATURES_REPLY;

        using iterator = std::vector<v10::port>::const_iterator;
        using const_iterator = std::vector<v10::port>::const_iterator;

        features_reply(features_request const& request
                     , std::uint64_t const dpid
                     , std::uint32_t const n_buffers
                     , std::uint8_t const n_tables
                     , std::uint32_t const capabilities
                     , std::uint32_t const actions
                     , std::vector<v10::port> ports)
            : switch_features_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(
                            sizeof(v10_detail::ofp_switch_features)
                          + ports.size() * sizeof(v10_detail::ofp_phy_port))
                    , request.xid()
                  }
                , dpid
                , n_buffers
                , n_tables
                , { 0, 0, 0 }
                , capabilities
                , actions
              }
            , ports_(std::move(ports))
        {
        }

        auto header() const noexcept
            -> v10_detail::ofp_header
        {
            return switch_features_.header;
        }

        auto datapath_id() const noexcept
            -> std::uint64_t
        {
            return switch_features_.datapath_id;
        }

        auto num_buffers() const noexcept
            -> std::uint32_t
        {
            return switch_features_.n_buffers;
        }

        auto num_tables() const noexcept
            -> std::uint8_t
        {
            return switch_features_.n_tables;
        }

        auto capabilities() const noexcept
            -> std::uint32_t
        {
            return switch_features_.capabilities;
        }

        auto actions() const noexcept
            -> std::uint32_t
        {
            return switch_features_.actions;
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return ports_.begin();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return ports_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, switch_features_);
            boost::for_each(ports_, [&](v10::port const& port) {
                port.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> features_reply
        {
            auto const features
                = detail::decode<v10_detail::ofp_switch_features>(first, last);
            auto ports = std::vector<port>{};
            ports.reserve(
                    (features.header.length - sizeof(features))
                    / sizeof(v10_detail::ofp_phy_port));
            while (first != last) {
                ports.emplace_back(port::decode(first, last));
            }
            return features_reply{features, std::move(ports)};
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < sizeof(v10_detail::ofp_switch_features)
                    || ((header.length - sizeof(v10_detail::ofp_switch_features))
                        % sizeof(v10_detail::ofp_phy_port) != 0)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        features_reply(v10_detail::ofp_switch_features const& features
                     , std::vector<port>&& ports)
            : switch_features_(features)
            , ports_(std::move(ports))
        {
        }

    private:
        v10_detail::ofp_switch_features switch_features_;
        std::vector<v10::port> ports_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_FEATURES_HPP
