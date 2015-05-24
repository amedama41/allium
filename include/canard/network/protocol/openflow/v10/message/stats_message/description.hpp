#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_DESCRIPTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_DESCRIPTION_HPP

#include <cstdint>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/message/stats_message/basic_stats_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class description_request
        : public messages_detail::basic_stats_request<description_request>
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_DESC;

        explicit description_request(std::uint32_t const xid = get_xid())
            : basic_stats_request{xid}
        {
        }

        using basic_stats_request::encode;
        using basic_stats_request::decode;

    private:
        friend basic_stats_request;
        explicit description_request(v10_detail::ofp_stats_request const& stats_request)
            : basic_stats_request{stats_request, 0}
        {
        }
    };

    class description_reply
        : public messages_detail::basic_stats_reply<description_reply, v10_detail::ofp_stats_reply>
    {
    public:
        static ofp_stats_types const stats_type_value = OFPST_DESC;

        auto manufacture_desc() const
            -> boost::string_ref
        {
            return desc_stats_.mfr_desc;
        }

        auto hardware_desc() const
            -> boost::string_ref
        {
            return desc_stats_.hw_desc;
        }

        auto software_desc() const
            -> boost::string_ref
        {
            return desc_stats_.sw_desc;
        }

        auto serial_number() const
            -> boost::string_ref
        {
            return desc_stats_.serial_num;
        }

        auto datapath_desc() const
            -> boost::string_ref
        {
            return desc_stats_.dp_desc;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(basic_stats_reply::encode(container), desc_stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> description_reply
        {
            auto const stats_reply = basic_stats_reply::decode(first, last);
            auto const desc_stats = detail::decode<v10_detail::ofp_desc_stats>(first, last);
            return description_reply{stats_reply, desc_stats};
        }

    private:
        description_reply(v10_detail::ofp_stats_reply const& stats_reply, v10_detail::ofp_desc_stats const& desc_stats)
            : basic_stats_reply{stats_reply, sizeof(desc_stats_)}
            , desc_stats_(desc_stats)
        {
        }

    private:
        v10_detail::ofp_desc_stats desc_stats_;
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_DESCRIPTION_HPP
