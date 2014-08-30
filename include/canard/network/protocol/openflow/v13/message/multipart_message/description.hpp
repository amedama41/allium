#ifndef CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP

#include <cstdint>
#include <iterator>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class description_request
        : public detail::basic_multipart_request<description_request>
    {
    public:
        static ofp_multipart_type const multipart_type_value = OFPMP_DESC;

        description_request()
            : basic_multipart_request{0, 0}
        {
        }
    };

    class description_reply
        : public detail::basic_multipart_reply<description_reply>
    {
    public:
        static ofp_multipart_type const multipart_type_value = OFPMP_DESC;

        auto manufacture_desc() const
            -> char const*
        {
            return desc_.mfr_desc;
        }

        auto hardware_desc() const
            -> char const*
        {
            return desc_.hw_desc;
        }

        auto software_desc() const
            -> char const*
        {
            return desc_.sw_desc;
        }

        auto serial_number() const
            -> char const*
        {
            return desc_.serial_num;
        }

        auto datapah_desc() const
            -> char const*
        {
            return desc_.dp_desc;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> description_reply
        {
            auto reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(detail::ofp_multipart_reply)) {
                throw 2;
            }
            if (std::distance(first, last) != sizeof(detail::ofp_desc)) {
                throw 2;
            }

            auto desc = detail::decode<detail::ofp_desc>(first, last);

            return description_reply{reply, desc};
        }

    private:
        description_reply(detail::ofp_multipart_reply const& reply, detail::ofp_desc const& desc)
            : basic_multipart_reply{reply}
            , desc_(desc)
        {
        }

    private:
        detail::ofp_desc desc_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP
