#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_DESCRIPTION_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_DESCRIPTION_HPP

#include <cstdint>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/stats_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    class description_request
        : public v10_detail::stats_request_adaptor<
                description_request, void
          >
    {
    public:
        static protocol::ofp_stats_types const stats_type_value
            = protocol::OFPST_DESC;

        explicit description_request(std::uint32_t const xid = get_xid())
            : stats_request_adaptor{xid}
        {
        }

    private:
        friend stats_request_adaptor;
        explicit description_request(v10_detail::ofp_stats_request const& stats_request)
            : stats_request_adaptor{stats_request}
        {
        }
    };


    class description_reply
        : public v10_detail::stats_reply_adaptor<
                description_reply, v10_detail::ofp_desc_stats, false
          >
    {
    public:
        static protocol::ofp_stats_types const stats_type_value
            = protocol::OFPST_DESC;

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

    private:
        friend stats_reply_adaptor;

        auto body() const
            -> v10_detail::ofp_desc_stats const&
        {
            return desc_stats_;
        }

        description_reply(
                  v10_detail::ofp_stats_reply const& stats_reply
                , v10_detail::ofp_desc_stats const& desc_stats)
            : stats_reply_adaptor{stats_reply}
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
