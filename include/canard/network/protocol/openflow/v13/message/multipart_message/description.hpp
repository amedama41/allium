#ifndef CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP

#include <cstdint>
#include <iterator>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class description_request
        : public v13_detail::basic_multipart_request<description_request>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_DESC;

        description_request()
            : basic_multipart_request{0, 0}
        {
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> description_request
        {
            return description_request{basic_multipart_request::decode(first, last)};
        }

    private:
        explicit description_request(v13_detail::ofp_multipart_request const& request)
            : basic_multipart_request{request}
        {
        }
    };

    class description_reply
        : public v13_detail::basic_multipart_reply<description_reply>
    {
    public:
        static protocol::ofp_multipart_type const multipart_type_value
            = protocol::OFPMP_DESC;

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

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_reply::encode(container);
            return detail::encode(container, desc_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> description_reply
        {
            auto reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(v13_detail::ofp_multipart_reply)) {
                throw 2;
            }
            if (std::distance(first, last) != sizeof(v13_detail::ofp_desc)) {
                throw 2;
            }

            auto desc = detail::decode<v13_detail::ofp_desc>(first, last);

            return description_reply{reply, desc};
        }

    private:
        description_reply(v13_detail::ofp_multipart_reply const& reply, v13_detail::ofp_desc const& desc)
            : basic_multipart_reply{reply}
            , desc_(desc)
        {
        }

    private:
        v13_detail::ofp_desc desc_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SWITCH_DESC_HPP
