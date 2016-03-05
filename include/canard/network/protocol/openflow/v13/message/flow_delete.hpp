#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_DELETE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_DELETE_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    namespace flow_mod_detail {

        template <class FlowDeleteType>
        class flow_delete_base
            : public v13_detail::basic_openflow_message<FlowDeleteType>
        {
            static constexpr auto base_flow_mod_size
                = sizeof(v13_detail::ofp_flow_mod)
                + sizeof(v13_detail::ofp_match);

        public:
            static constexpr protocol::ofp_type message_type
                = protocol::OFPT_FLOW_MOD;

            auto header() const noexcept
                -> v13_detail::ofp_header const&
            {
                return flow_mod_.header;
            }

            auto match() const noexcept
                -> oxm_match_set const&
            {
                return match_;
            }

            auto cookie() const noexcept
                -> std::uint64_t
            {
                return flow_mod_.cookie;
            }

            auto cookie_mask() const noexcept
                -> std::uint64_t
            {
                return flow_mod_.cookie_mask;
            }

            auto table_id() const noexcept
                -> std::uint8_t
            {
                return flow_mod_.table_id;
            }

            auto out_port() const noexcept
                -> std::uint32_t
            {
                return flow_mod_.out_port;
            }

            auto out_group() const noexcept
                -> std::uint32_t
            {
                return flow_mod_.out_group;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, flow_mod_);
                return match_.encode(container);
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> FlowDeleteType
            {
                auto const flow_mod
                    = detail::decode<v13_detail::ofp_flow_mod>(first, last);
                last = std::next(
                        first
                      , flow_mod.header.length - sizeof(v13_detail::ofp_flow_mod));

                auto copy_first = first;
                auto const ofp_match
                    = detail::decode<v13_detail::ofp_match>(copy_first, last);
                oxm_match_set::validate(ofp_match);
                if (std::distance(first, last)
                        != v13_detail::exact_length(ofp_match.length)) {
                    throw std::runtime_error{"oxm_match length is too big"};
                }
                auto match = oxm_match_set::decode(first, last);

                return FlowDeleteType{flow_mod, std::move(match)};
            }

            static void validate(v13_detail::ofp_header const& header)
            {
                if (header.version != protocol::OFP_VERSION) {
                    throw std::runtime_error{"invalid version"};
                }
                if (header.type != message_type) {
                    throw std::runtime_error{"invalid message type"};
                }
                if (header.length < base_flow_mod_size) {
                    throw std::runtime_error{"invalid length"};
                }
            }

        protected:
            flow_delete_base(
                      oxm_match_set&& match
                    , std::uint16_t const priority
                    , std::uint64_t const cookie
                    , std::uint64_t const cookie_mask
                    , std::uint8_t const table_id
                    , std::uint32_t const out_port
                    , std::uint32_t const out_group
                    , std::uint32_t const xid)
                : flow_mod_{
                      v13_detail::ofp_header{
                          protocol::OFP_VERSION
                        , message_type
                        , std::uint16_t(
                                  sizeof(v13_detail::ofp_flow_mod)
                                + v13_detail::exact_length(match.length()))
                        , xid
                      }
                    , cookie
                    , cookie_mask
                    , table_id
                    , FlowDeleteType::command_type
                    , 0
                    , 0
                    , priority
                    , 0
                    , out_port
                    , out_group
                    , 0
                    , { 0, 0 }
                  }
                , match_(std::move(match))
            {
            }

            flow_delete_base(
                      v13_detail::ofp_flow_mod const& flow_mod
                    , oxm_match_set&& match)
                : flow_mod_(flow_mod)
                , match_(std::move(match))
            {
            }

            flow_delete_base(flow_delete_base const&) = default;

            flow_delete_base(flow_delete_base&& other)
                : flow_mod_(other.flow_mod_)
                , match_(std::move(other).match_)
            {
                other.flow_mod_.header.length = base_flow_mod_size;
            }

            auto operator=(flow_delete_base const&)
                -> flow_delete_base& = default;

            auto operator=(flow_delete_base&& other)
                -> flow_delete_base&
            {
                auto tmp = std::move(other);
                std::swap(flow_mod_, tmp.flow_mod_);
                match_.swap(tmp.match_);
                return *this;
            }

            auto ofp_flow_mod() const noexcept
                -> v13_detail::ofp_flow_mod const&
            {
                return flow_mod_;
            }

        private:
            v13_detail::ofp_flow_mod flow_mod_;
            oxm_match_set match_;
        };

    } // namespace flow_mod_detail


    class flow_delete
        : public flow_mod_detail::flow_delete_base<flow_delete>
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_DELETE;

        flow_delete(oxm_match_set match
                  , std::uint8_t const table_id
                  , v13::cookie_mask const cookie_mask
                  , std::uint32_t const out_port = protocol::OFPP_ANY
                  , std::uint32_t const out_group = protocol::OFPG_ANY
                  , std::uint32_t const xid = get_xid())
            : flow_delete_base{
                  std::move(match)
                , 0
                , cookie_mask.value()
                , cookie_mask.mask()
                , table_id
                , out_port
                , out_group
                , xid
              }
        {
        }

        flow_delete(oxm_match_set match
                  , std::uint8_t const table_id
                  , std::uint32_t const out_port = protocol::OFPP_ANY
                  , std::uint32_t const out_group = protocol::OFPG_ANY
                  , std::uint32_t const xid = get_xid())
            : flow_delete{
                  std::move(match)
                , table_id
                , v13::cookie_mask{0, 0}
                , out_port
                , out_group
                , xid
              }
        {
        }

    private:
        friend flow_delete_base;

        flow_delete(v13_detail::ofp_flow_mod const& flow_mod
                  , oxm_match_set&& match)
            : flow_delete_base{flow_mod, std::move(match)}
        {
        }
    };


    class flow_delete_strict
        : public flow_mod_detail::flow_delete_base<flow_delete_strict>
    {
    public:
        static constexpr protocol::ofp_flow_mod_command command_type
            = protocol::OFPFC_DELETE_STRICT;

        flow_delete_strict(
                  flow_entry entry
                , std::uint8_t const table_id
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : flow_delete_base{
                  std::move(entry).match()
                , entry.priority()
                , entry.cookie()
                , std::numeric_limits<std::uint64_t>::max()
                , table_id
                , out_port
                , out_group
                , xid
              }
        {
        }

        flow_delete_strict(
                  oxm_match_set match
                , std::uint16_t const priority
                , std::uint8_t const table_id
                , v13::cookie_mask const& cookie_mask
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : flow_delete_base{
                  std::move(match)
                , priority
                , cookie_mask.value()
                , cookie_mask.mask()
                , table_id
                , out_port
                , out_group
                , xid
              }
        {
        }

        flow_delete_strict(
                  oxm_match_set match
                , std::uint16_t const priority
                , std::uint8_t const table_id
                , std::uint32_t const out_port = protocol::OFPP_ANY
                , std::uint32_t const out_group = protocol::OFPG_ANY
                , std::uint32_t const xid = get_xid())
            : flow_delete_strict{
                  std::move(match)
                , priority
                , table_id
                , v13::cookie_mask{0, 0}
                , out_port
                , out_group
                , xid
              }
        {
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return ofp_flow_mod().priority;
        }

        auto id() const
            -> flow_entry_id
        {
            return flow_entry_id{match(), priority()};
        }

    private:
        friend flow_delete_base;

        flow_delete_strict(
                  v13_detail::ofp_flow_mod const& flow_mod
                , oxm_match_set&& match)
            : flow_delete_base{flow_mod, std::move(match)}
        {
        }
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_FLOW_DELETE_HPP
