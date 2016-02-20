#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP

#include <cstdint>
#include <stdexcept>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/flow_entry.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {

    namespace flow_mod_detail {

        template <class FlowDeleteType>
        class flow_delete_base
            : public v10_detail::basic_openflow_message<FlowDeleteType>
        {
        public:
            static constexpr protocol::ofp_type message_type
                = protocol::OFPT_FLOW_MOD;

            auto header() const noexcept
                -> v10_detail::ofp_header const&
            {
                return flow_mod_.header;
            }

            auto match() const noexcept
                -> match_set
            {
                return match_set{flow_mod_.match};
            }

            auto out_port() const noexcept
                -> std::uint16_t
            {
                return flow_mod_.out_port;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                return detail::encode(container, flow_mod_);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> FlowDeleteType
            {
                return FlowDeleteType{
                    detail::decode<v10_detail::ofp_flow_mod>(first, last)
                };
            }

            static void validate(v10_detail::ofp_header const& header)
            {
                if (header.version != protocol::OFP_VERSION) {
                    throw std::runtime_error{"invalid version"};
                }
                if (header.type != message_type) {
                    throw std::runtime_error{"invalid message type"};
                }
                if (header.length != sizeof(v10_detail::ofp_flow_mod)) {
                    throw std::runtime_error{"invalid length"};
                }
            }

        protected:
            flow_delete_base(
                      match_set const& match
                    , std::uint16_t const priority
                    , std::uint16_t const out_port
                    , std::uint32_t const xid) noexcept
                : flow_mod_{
                      v10_detail::ofp_header{
                          protocol::OFP_VERSION
                        , message_type
                        , sizeof(v10_detail::ofp_flow_mod)
                        , xid
                      }
                    , match.ofp_match()
                    , 0
                    , FlowDeleteType::command_type
                    , 0
                    , 0
                    , priority
                    , 0
                    , out_port
                    , 0
                  }
            {
            }

            explicit flow_delete_base(
                    v10_detail::ofp_flow_mod const& flow_mod) noexcept
                : flow_mod_(flow_mod)
            {
            }

            auto ofp_flow_mod() const noexcept
                -> v10_detail::ofp_flow_mod const&
            {
                return flow_mod_;
            }

        private:
            v10_detail::ofp_flow_mod flow_mod_;
        };

    } // namespace flow_mod_detail


    class flow_delete
        : public flow_mod_detail::flow_delete_base<flow_delete>
    {
    public:
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE;

        explicit flow_delete(
                  match_set const& match
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid()) noexcept
            : flow_delete_base{match, 0, out_port, xid}
        {
        }

    private:
        friend flow_delete_base;

        explicit flow_delete(v10_detail::ofp_flow_mod const& flow_mod) noexcept
            : flow_delete_base{flow_mod}
        {
        }
    };


    class flow_delete_strict
        : public flow_mod_detail::flow_delete_base<flow_delete_strict>
    {
    public:
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE_STRICT;

        flow_delete_strict(
                  match_set const& match
                , std::uint16_t const priority
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid()) noexcept
            : flow_delete_base{match, priority, out_port, xid}
        {
        }

        explicit flow_delete_strict(
                  flow_entry const& entry
                , std::uint16_t const out_port = protocol::OFPP_NONE
                , std::uint32_t const xid = get_xid()) noexcept
            : flow_delete_base{entry.match(), entry.priority(), out_port, xid}
        {
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return ofp_flow_mod().priority;
        }

        auto id() const noexcept
            -> flow_entry_id
        {
            return flow_entry_id{match(), priority()};
        }

    private:
        friend flow_delete_base;

        explicit flow_delete_strict(
                v10_detail::ofp_flow_mod const& flow_mod) noexcept
            : flow_delete_base{flow_mod}
        {
        }
    };

} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_FLOW_DELETE_HPP
