#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_CONFIG_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_CONFIG_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    template <class T>
    class switch_config_base
        : public v10_detail::basic_openflow_message<T>
    {
        using base_type = v10_detail::basic_openflow_message<T>;

    protected:
        switch_config_base(ofp_config_flags const flags, std::uint16_t const miss_send_len
                , std::uint32_t const xid = get_xid())
            : config_{
                  {OFP_VERSION, T::message_type, sizeof(config_), xid}
                , std::uint16_t(flags), miss_send_len
              }
        {
        }

        explicit switch_config_base(v10_detail::ofp_switch_config const& config)
            : config_(config)
        {
            if (base_type::version() != OFP_VERSION) {
                throw std::runtime_error("invalid version");
            }
            if (base_type::type() != T::message_type) {
                throw std::runtime_error("invalid message type");
            }
            if (base_type::length() != sizeof(v10_detail::ofp_switch_config)) {
                throw std::runtime_error("invalid length");
            }
        }

    public:
        auto header() const
            -> v10_detail::ofp_header
        {
            return config_.header;
        }

        auto flags() const
            -> std::uint16_t
        {
            return ofp_config_flags(config_.flags);
        }

        auto miss_send_length() const
            -> std::uint16_t
        {
            return config_.miss_send_len;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, config_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const config
                = detail::decode<v10_detail::ofp_switch_config>(first, last);
            return T{config};
        }

    private:
        v10_detail::ofp_switch_config config_;
    };


    class get_config_request
        : public v10_detail::basic_openflow_message<get_config_request>
    {
    public:
        static ofp_type const message_type = OFPT_GET_CONFIG_REQUEST;

        explicit get_config_request(std::uint32_t const xid = get_xid())
            : header_{OFP_VERSION, message_type, sizeof(header_), xid}
        {
        }

        auto header() const
            -> v10_detail::ofp_header
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
        auto decode(Iterator& first, Iterator last)
            -> get_config_request
        {
            auto const header
                = detail::decode<v10_detail::ofp_header>(first, last);
            return get_config_request{header};
        }

    private:
        explicit get_config_request(v10_detail::ofp_header const header)
            : header_(header)
        {
            if (version() != OFP_VERSION) {
                throw std::runtime_error("invalid version");
            }
            if (type() != message_type) {
                throw std::runtime_error("invalid message type");
            }
            if (length() != sizeof(v10_detail::ofp_header)) {
                throw std::runtime_error("invalid length");
            }
        }

    private:
        v10_detail::ofp_header header_;
    };


    class get_config_reply
        : public switch_config_base<get_config_reply>
    {
    public:
        static ofp_type const message_type = OFPT_GET_CONFIG_REPLY;

    private:
        friend switch_config_base;

        explicit get_config_reply(v10_detail::ofp_switch_config const& config)
            : switch_config_base{config}
        {
        }

    private:
        v10_detail::ofp_switch_config config_;
    };


    class set_config
        : public switch_config_base<set_config>
    {
    public:
        static ofp_type const message_type = OFPT_SET_CONFIG;

        set_config(ofp_config_flags const flags, std::uint16_t const miss_send_len
                , std::uint32_t const xid = get_xid())
            : switch_config_base{flags, miss_send_len, xid}
        {
        }

        explicit set_config(std::uint16_t const miss_send_len, std::uint32_t const xid = get_xid())
            : set_config{OFPC_FRAG_NORMAL, miss_send_len, xid}
        {
        }

    private:
        friend switch_config_base;

        explicit set_config(v10_detail::ofp_switch_config const& config)
            : switch_config_base{config}
        {
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_SWITCH_CONFIG_HPP
