#ifndef CANARD_NETWORK_OPENFLOW_V13_SWITCH_CONFIG_HPP
#define CANARD_NETWORK_OPENFLOW_V13_SWITCH_CONFIG_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    class switch_config_request
        : public v13_detail::basic_openflow_message<switch_config_request>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_GET_CONFIG_REQUEST;

        switch_config_request()
            : header_{
                  protocol::OFP_VERSION, message_type
                , sizeof(v13_detail::ofp_header), get_xid()
              }
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
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
            -> switch_config_request
        {
            return switch_config_request{detail::decode<v13_detail::ofp_header>(first, last)};
        }

    private:
        explicit switch_config_request(v13_detail::ofp_header const& header)
            : header_(header)
        {
        }

    private:
        v13_detail::ofp_header header_;
    };

    namespace switch_config_detail {

        template <class T>
        class basic_switch_config
             : public v13_detail::basic_openflow_message<T>
        {
        protected:
            basic_switch_config(std::uint16_t const flags, std::uint16_t const miss_send_len)
                : switch_config_{
                      v13_detail::ofp_header{
                          protocol::OFP_VERSION, T::message_type
                        , sizeof(v13_detail::ofp_switch_config)
                        , v13_detail::basic_openflow_message<T>::get_xid()
                      }
                    , flags, miss_send_len
                  }
            {
            }

        public:
            auto header() const
                -> v13_detail::ofp_header const&
            {
                return switch_config_.header;
            }

            auto flags() const
                -> std::uint16_t
            {
                return switch_config_.flags;
            }

            auto miss_send_length() const
                -> std::uint16_t
            {
                return switch_config_.miss_send_len;
            }

            using v13_detail::basic_openflow_message<T>::encode;

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, switch_config_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> T
            {
                return T{detail::decode<v13_detail::ofp_switch_config>(first, last)};
            }

        protected:
            explicit basic_switch_config(v13_detail::ofp_switch_config const& config)
                : switch_config_(config)
            {
            }

        private:
            v13_detail::ofp_switch_config switch_config_;
        };

    } // namespace v13_detail

    class switch_config_reply
        : public switch_config_detail::basic_switch_config<switch_config_reply>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_GET_CONFIG_REPLY;

        switch_config_reply(std::uint16_t const flags, std::uint16_t const miss_send_len)
            : basic_switch_config{flags, miss_send_len}
        {
        }

    private:
        friend basic_switch_config;

        explicit switch_config_reply(v13_detail::ofp_switch_config const& config)
            : basic_switch_config{config}
        {
        }
    };

    class set_switch_config
        : public switch_config_detail::basic_switch_config<set_switch_config>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_SET_CONFIG;

        set_switch_config(std::uint16_t const flags, std::uint16_t const miss_send_len)
            : basic_switch_config{flags, miss_send_len}
        {
        }

    private:
        friend basic_switch_config;

        explicit set_switch_config(v13_detail::ofp_switch_config const& config)
            : basic_switch_config{config}
        {
        }
    };

} // namespace messages

using messages::switch_config_request;
using messages::switch_config_reply;
using messages::set_switch_config;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_SWITCH_CONFIG_HPP
