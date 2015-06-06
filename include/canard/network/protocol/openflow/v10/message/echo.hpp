#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGE_ECHO_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGE_ECHO_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    namespace echo_detail {

        template <class T>
        class echo_base
            : public v10_detail::basic_openflow_message<T>
        {
        protected:
            echo_base(std::vector<std::uint8_t> data, std::uint32_t const xid)
                : header_{
                      OFP_VERSION, T::message_type
                    , std::uint16_t(sizeof(header_) + data.size())
                    , xid
                  }
                , data_(std::move(data))
            {
            }

        public:
            auto header() const
                -> v10_detail::ofp_header
            {
                return header_;
            }

            auto data() const&
                -> std::vector<std::uint8_t> const&
            {
                return data_;
            }

            auto data() &&
                -> std::vector<std::uint8_t>
            {
                auto data = std::move(data_);
                header_.length -= data_.size();
                return data;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, header_);
                return container.push_back(data_.data(), data_.size());
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> T
            {
                auto const header = detail::decode<v10_detail::ofp_header>(first, last);
                auto data = std::vector<std::uint8_t>(first, last);
                return T{header, std::move(data)};
            }

        protected:
            echo_base(v10_detail::ofp_header const header
                    , std::vector<std::uint8_t> data)
                : header_(header)
                , data_(std::move(data))
            {
            }

        private:
            v10_detail::ofp_header header_;
            std::vector<std::uint8_t> data_;
        };

    } // namespace echo_detail


    class echo_request
        : public echo_detail::echo_base<echo_request>
    {
    public:
        static ofp_type const message_type = OFPT_ECHO_REQUEST;

        explicit echo_request(std::uint32_t const xid = get_xid())
            : echo_base{{}, xid}
        {
        }

        explicit echo_request(
                  std::vector<std::uint8_t> data
                , std::uint32_t const xid = get_xid())
            : echo_base{std::move(data), xid}
        {
        }

    private:
        friend echo_base;

        echo_request(
                  v10_detail::ofp_header const header
                , std::vector<std::uint8_t> data)
            : echo_base{header, std::move(data)}
        {
        }
    };


    class echo_reply
        : public echo_detail::echo_base<echo_reply>
    {
    public:
        static ofp_type const message_type = OFPT_ECHO_REPLY;

        explicit echo_reply(echo_request request)
            : echo_base{std::move(request).data(), request.xid()}
        {
        }

    private:
        friend echo_base;

        echo_reply(
                  v10_detail::ofp_header const header
                , std::vector<std::uint8_t> data)
            : echo_base{header, std::move(data)}
        {
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGE_ECHO_HPP
