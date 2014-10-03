#ifndef CANARD_NETWORK_OPENFLOW_V13_SWITCH_FEATURES
#define CANARD_NETWORK_OPENFLOW_V13_SWITCH_FEATURES

#include <cstdint>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class features_request
        : public detail::basic_openflow_message<features_request>
    {
    public:
        static ofp_type const message_type = OFPT_FEATURES_REQUEST;

        features_request()
            : header_{OFP_VERSION, message_type, sizeof(detail::ofp_header), get_xid()}
        {
        }

        auto header() const
            -> detail::ofp_header const&
        {
            return header_;
        }

        using basic_openflow_message::encode;

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
            auto const header = detail::decode<detail::ofp_header>(first, last);
            if (header.length != sizeof(detail::ofp_header) || std::distance(first, last) == 0) {
                throw 2;
            }
            return features_request{header};
        }

    private:
        explicit features_request(detail::ofp_header const& header)
            : header_(header)
        {
        }

    private:
        detail::ofp_header header_;
    };


    class features_reply
        : public detail::basic_openflow_message<features_reply>
    {
    public:
        static ofp_type const message_type = OFPT_FEATURES_REPLY;

        features_reply(features_request const& request, std::uint64_t const dpid)
            : switch_features_{
                  {OFP_VERSION, message_type, sizeof(detail::ofp_switch_features), request.xid()}
                , dpid
                , 0 // n_buffers
                , 0 // n_tables
                , 0 // auxiliary_id
                , {0, 0}
                , 0 // capabilities
                , 0
            }
        {
        }

        auto header() const
            -> detail::ofp_header const&
        {
            return switch_features_.header;
        }

        auto datapath_id() const
            -> std::uint64_t
        {
            return switch_features_.datapath_id;
        }

        auto num_buffers() const
            -> std::uint32_t
        {
            return switch_features_.n_buffers;
        }

        auto num_tables() const
            -> std::uint8_t
        {
            return switch_features_.n_tables;
        }

        auto auxiliary_id() const
            -> std::uint8_t
        {
            return switch_features_.auxiliary_id;
        }

        using basic_openflow_message::encode;

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
            auto const switch_features = detail::decode<detail::ofp_switch_features>(first, last);
            if (switch_features.header.length != sizeof(detail::ofp_switch_features) || std::distance(first, last) != 0) {
                throw 2;
            }
            return features_reply{switch_features};
        }

    private:
        explicit features_reply(detail::ofp_switch_features const& switch_features)
            : switch_features_(switch_features)
        {
        }

    private:
        detail::ofp_switch_features switch_features_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SWITCH_FEATURES
