#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_SWITCH_FEATURES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_SWITCH_FEATURES_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class features_request
        : public v13_detail::basic_openflow_message<features_request>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FEATURES_REQUEST;

        features_request(std::uint32_t const xid = get_xid()) noexcept
            : header_{
                  protocol::OFP_VERSION
                , message_type
                , sizeof(v13_detail::ofp_header)
                , xid
              }
        {
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
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
                = detail::decode<v13_detail::ofp_header>(first, last);
            return features_request{header};
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid type"};
            }
            if (header.length != sizeof(v13_detail::ofp_header)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit features_request(v13_detail::ofp_header const& header) noexcept
            : header_(header)
        {
        }

    private:
        v13_detail::ofp_header header_;
    };


    class features_reply
        : public v13_detail::basic_openflow_message<features_reply>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FEATURES_REPLY;

        features_reply(features_request const& request
                     , std::uint64_t const dpid
                     , std::uint32_t const n_buffers
                     , std::uint8_t const n_tables
                     , std::uint8_t const auxiliary_id
                     , std::uint32_t const capabilities) noexcept
            : switch_features_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , sizeof(v13_detail::ofp_switch_features)
                    , request.xid()
                  }
                , dpid
                , n_buffers
                , n_tables
                , auxiliary_id
                , { 0, 0 }
                , capabilities
                , 0
              }
        {
        }

        auto header() const noexcept
            -> v13_detail::ofp_header const&
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

        auto auxiliary_id() const noexcept
            -> std::uint8_t
        {
            return switch_features_.auxiliary_id;
        }

        auto capabilities() const noexcept
            -> std::uint32_t
        {
            return switch_features_.capabilities;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, switch_features_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> features_reply
        {
            auto const switch_features
                = detail::decode<v13_detail::ofp_switch_features>(first, last);
            return features_reply{switch_features};
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid type"};
            }
            if (header.length != sizeof(v13_detail::ofp_switch_features)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    private:
        explicit features_reply(
                v13_detail::ofp_switch_features const& switch_features) noexcept
            : switch_features_(switch_features)
        {
        }

    private:
        v13_detail::ofp_switch_features switch_features_;
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_SWITCH_FEATURES_HPP
