#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_DESCRIPTION_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_DESCRIPTION_HPP

#include <cstdint>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {

    class description_request
        : public multipart_detail::basic_multipart_request<
              description_request, void
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_DESC;

        explicit description_request(
                std::uint32_t const xid = get_xid()) noexcept
            : basic_multipart_request{0, xid}
        {
        }

    private:
        friend basic_multipart_request::base_type;

        explicit description_request(
                v13_detail::ofp_multipart_request const& request) noexcept
            : basic_multipart_request{request}
        {
        }
    };


    class description_reply
        : public multipart_detail::basic_multipart_reply<
                description_reply, v13_detail::ofp_desc
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_DESC;

        description_reply(
                  boost::string_ref mfr_desc
                , boost::string_ref hw_desc
                , boost::string_ref sw_desc
                , boost::string_ref serial_num
                , boost::string_ref dp_desc
                , std::uint32_t const xid = get_xid())
            : basic_multipart_reply{
                  0
                , create_desc(mfr_desc, hw_desc, sw_desc, serial_num, dp_desc)
                , xid
              }
        {
        }

        auto manufacture_desc() const
            -> boost::string_ref
        {
            return body().mfr_desc;
        }

        auto hardware_desc() const
            -> boost::string_ref
        {
            return body().hw_desc;
        }

        auto software_desc() const
            -> boost::string_ref
        {
            return body().sw_desc;
        }

        auto serial_number() const
            -> boost::string_ref
        {
            return body().serial_num;
        }

        auto datapath_desc() const
            -> boost::string_ref
        {
            return body().dp_desc;
        }

    private:
        friend basic_multipart_reply::base_type;

        description_reply(
                  v13_detail::ofp_multipart_reply const& reply
                , v13_detail::ofp_desc const& desc) noexcept
            : basic_multipart_reply{reply, desc}
        {
        }

        static auto create_desc(
                  boost::string_ref mfr_desc
                , boost::string_ref hw_desc
                , boost::string_ref sw_desc
                , boost::string_ref serial
                , boost::string_ref dp_desc)
            -> v13_detail::ofp_desc
        {
            using boost::adaptors::sliced;

            auto desc = v13_detail::ofp_desc{};

            auto const mfr_desc_size
                = std::min(mfr_desc.size(), sizeof(desc.mfr_desc) - 1);
            boost::copy(mfr_desc | sliced(0, mfr_desc_size), desc.mfr_desc);

            auto const hw_desc_size
                = std::min(hw_desc.size(), sizeof(desc.hw_desc) - 1);
            boost::copy(hw_desc | sliced(0, hw_desc_size), desc.hw_desc);

            auto const sw_desc_size
                = std::min(sw_desc.size(), sizeof(desc.sw_desc) - 1);
            boost::copy(sw_desc | sliced(0, sw_desc_size), desc.sw_desc);

            auto const serial_size
                = std::min(serial.size(), sizeof(desc.serial_num) - 1);
            boost::copy(serial | sliced(0, serial_size), desc.serial_num);

            auto const dp_desc_size
                = std::min(dp_desc.size(), sizeof(desc.dp_desc) - 1);
            boost::copy(dp_desc | sliced(0, dp_desc_size), desc.dp_desc);

            return desc;
        }
    };

} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_DESCRIPTION_HPP
