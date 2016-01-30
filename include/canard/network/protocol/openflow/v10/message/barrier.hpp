#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    namespace barrier_detail {

        template <class T>
        class barrier_base
            : public v10_detail::basic_openflow_message<T>
        {
        protected:
            explicit barrier_base(std::uint32_t const xid) noexcept
                : header_{
                      protocol::OFP_VERSION
                    , T::message_type
                    , sizeof(v10_detail::ofp_header)
                    , xid
                  }
            {
            }

        public:
            auto header() const noexcept
                -> v10_detail::ofp_header const&
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
                -> T
            {
                return T{detail::decode<v10_detail::ofp_header>(first, last)};
            }

            static void validate(v10_detail::ofp_header const& header)
            {
                if (header.version != protocol::OFP_VERSION) {
                    throw std::runtime_error{"invalid version"};
                }
                if (header.type != T::message_type) {
                    throw std::runtime_error{"invalid message type"};
                }
                if (header.length != sizeof(v10_detail::ofp_header)) {
                    throw std::runtime_error{"invalid length"};
                }
            }

        protected:
            explicit barrier_base(v10_detail::ofp_header const& header) noexcept
                : header_(header)
            {
            }

        private:
            v10_detail::ofp_header header_;
        };

    } // namespace barrier_detail

    class barrier_request
        : public barrier_detail::barrier_base<barrier_request>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_BARRIER_REQUEST;

        explicit barrier_request(std::uint32_t const xid = get_xid()) noexcept
            : barrier_base{xid}
        {
        }

    private:
        friend barrier_base;

        explicit barrier_request(v10_detail::ofp_header const& header) noexcept
            : barrier_base{header}
        {
        }
    };


    class barrier_reply
        : public barrier_detail::barrier_base<barrier_reply>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_BARRIER_REPLY;

        explicit barrier_reply(std::uint32_t const xid = get_xid()) noexcept
            : barrier_base{xid}
        {
        }

        explicit barrier_reply(barrier_request const& request)
            : barrier_reply{request.xid()}
        {
        }

    private:
        friend barrier_base;

        explicit barrier_reply(v10_detail::ofp_header const& header) noexcept
            : barrier_base{header}
        {
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_BARRIER_HPP
