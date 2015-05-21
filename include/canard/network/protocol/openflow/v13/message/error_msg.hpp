#ifndef CANARD_NETWORK_OPENFLOW_V13_ERROR_MSG_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ERROR_MSG_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/copy_n.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class error_msg
        : public v13_detail::basic_openflow_message<error_msg>
    {
    public:
        static ofp_type const message_type = OFPT_ERROR;

        error_msg(ofp_error_type const type, std::uint16_t const code
                , std::vector<unsigned char> data)
            : error_msg_{
                  { OFP_VERSION, message_type, std::uint16_t(sizeof(v13_detail::ofp_error_msg) + data.size()), get_xid() }
                , std::uint16_t(type), code
              }
            , data_(std::move(data))
        {
            if (data_.size() < sizeof(v13_detail::ofp_header)) {
                throw 2;
            }
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return error_msg_.header;
        }

        auto error_type() const
            -> ofp_error_type
        {
            return ofp_error_type(error_msg_.type);
        }

        auto error_code() const
            -> std::uint16_t
        {
            return error_msg_.code;
        }

        auto data() const
            -> std::vector<unsigned char> const&
        {
            return data_;
        }

        auto failed_request_header() const
            -> v13_detail::ofp_header
        {
            auto header = v13_detail::ofp_header();
            boost::copy_n(data_, sizeof(header), canard::as_byte_range(header).begin());
            return v13_detail::ntoh(header);
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, error_msg_);
            return boost::push_back(container, data_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> error_msg
        {
            auto const error = detail::decode<v13_detail::ofp_error_msg>(first, last);
            if (std::distance(first, last) != error.header.length - sizeof(v13_detail::ofp_error_msg)) {
                throw 2;
            }
            if (error.header.length < sizeof(v13_detail::ofp_error_msg) + sizeof(v13_detail::ofp_header)) {
                throw 3;
            }
            auto data = std::vector<unsigned char>(first, std::next(first, error.header.length - sizeof(v13_detail::ofp_error_msg)));
            std::advance(first, data.size());

            return {error, std::move(data)};
        }

    private:
        error_msg(v13_detail::ofp_error_msg const& error, std::vector<unsigned char> data)
            : error_msg_(error)
            , data_{std::move(data)}
        {
        }

    private:
        v13_detail::ofp_error_msg error_msg_;
        std::vector<unsigned char> data_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ERROR_MSG_HPP
