#ifndef CANARD_NETWORK_OPENFLOW_V13_BARRIER_HPP
#define CANARD_NETWORK_OPENFLOW_V13_BARRIER_HPP

#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class barrier_request
        : public detail::basic_openflow_message<barrier_request>
    {
    public:
        static ofp_type const message_type = OFPT_BARRIER_REQUEST;

        barrier_request()
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
            -> barrier_request
        {
            auto header = detail::decode<detail::ofp_header>(first, last);
            if (header.length != sizeof(detail::ofp_header) || std::distance(first, last) != 0) {
                throw 2;
            }
            return barrier_request{header};
        }

    private:
        barrier_request(detail::ofp_header const& header)
            : header_(header)
        {
        }

    private:
        detail::ofp_header header_;
    };

    class barrier_reply
        : public detail::basic_openflow_message<barrier_reply>
    {
    public:
        static ofp_type const message_type = OFPT_BARRIER_REPLY;

        barrier_reply(barrier_request const& request)
            : header_{OFP_VERSION, message_type, sizeof(detail::ofp_header), request.xid()}
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

    public:
        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> barrier_reply
        {
            auto const header = detail::decode<detail::ofp_header>(first, last);
            if (header.length != sizeof(detail::ofp_header) && std::distance(first, last) != 0) {
                throw 2;
            }
            return barrier_reply{header};
        }

    private:
        explicit barrier_reply(detail::ofp_header const& header)
            : header_(header)
        {
        }

    private:
        detail::ofp_header header_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_BARRIER_HPP
