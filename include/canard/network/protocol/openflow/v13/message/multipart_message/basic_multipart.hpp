#ifndef CANARD_NETWORK_OPENFLOW_V13_BASIC_MULTIPART_HPP
#define CANARD_NETWORK_OPENFLOW_V13_BASIC_MULTIPART_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <limits>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        inline auto over_64kb(std::size_t const body_length)
            -> bool
        {
            return sizeof(v13_detail::ofp_multipart_request) + body_length > std::numeric_limits<std::uint16_t>::max();
        }

        template <class T>
        class basic_multipart_request
            : public v13_detail::basic_openflow_message<T>
        {
        public:
            static protocol::ofp_type const message_type
                = protocol::OFPT_MULTIPART_REQUEST;

        protected:
            basic_multipart_request(std::size_t const body_length, std::uint16_t const flags)
                : request_{
                      v13_detail::ofp_header{
                            protocol::OFP_VERSION
                          , protocol::OFPT_MULTIPART_REQUEST
                          , v13_detail::over_64kb(body_length)
                              ? std::numeric_limits<std::uint16_t>::max()
                              : std::uint16_t(sizeof(v13_detail::ofp_multipart_request) + body_length)
                          , v13_detail::basic_openflow_message<T>::get_xid()
                      }
                    , T::multipart_type_value
                    , flags
                    , {0, 0, 0, 0}
                }
            {
            }

        public:
            auto header() const
                -> v13_detail::ofp_header const&
            {
                return request_.header;
            }

            auto multipart_type() const
                -> protocol::ofp_multipart_type
            {
                return protocol::ofp_multipart_type(request_.type);
            }

            auto flags() const
                -> std::uint16_t
            {
                return request_.flags;
            }

            using v13_detail::basic_openflow_message<T>::encode;

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, request_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> v13_detail::ofp_multipart_request
            {
                return detail::decode<v13_detail::ofp_multipart_request>(first, last);
            }

        protected:
            explicit basic_multipart_request(v13_detail::ofp_multipart_request const& request)
                : request_(request)
            {
            }

        private:
            v13_detail::ofp_multipart_request request_;
        };

        template <class T>
        class basic_multipart_reply
            : public v13_detail::basic_openflow_message<T>
        {
        public:
            static protocol::ofp_type const message_type
                = protocol::OFPT_MULTIPART_REPLY;

        protected:
            basic_multipart_reply(std::size_t const body_length, std::uint16_t flags)
                : reply_{
                      v13_detail::ofp_header{
                            protocol::OFP_VERSION
                          , protocol::OFPT_MULTIPART_REPLY
                          , v13_detail::over_64kb(body_length)
                              ? std::numeric_limits<std::uint16_t>::max()
                              : std::uint16_t(sizeof(v13_detail::ofp_multipart_reply) + body_length)
                          , v13_detail::basic_openflow_message<T>::get_xid()
                      }
                    , T::multipart_type_value
                    , flags
                    , {0, 0, 0, 0}
                }
            {
            }

        public:
            auto header() const
                -> v13_detail::ofp_header const&
            {
                return reply_.header;
            }

            auto multipart_type() const
                -> protocol::ofp_multipart_type
            {
                return protocol::ofp_multipart_type(reply_.type);
            }

            auto flags() const
                -> std::uint16_t
            {
                return reply_.flags;
            }

            using v13_detail::basic_openflow_message<T>::encode;

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, reply_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> v13_detail::ofp_multipart_reply
            {
                return detail::decode<v13_detail::ofp_multipart_reply>(first, last);
            }

        protected:
            explicit basic_multipart_reply(v13_detail::ofp_multipart_reply const& reply)
                : reply_(reply)
            {
                if (v13_detail::basic_openflow_message<T>::type() != message_type) {
                    throw std::runtime_error{"invalid type"};
                }

                if (multipart_type() != T::multipart_type_value) {
                    throw std::runtime_error{"invalid multipart type"};
                }
            }

        private:
            v13_detail::ofp_multipart_reply reply_;
        };

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_BASIC_MULTIPART_HPP
// vim: path+=../../
