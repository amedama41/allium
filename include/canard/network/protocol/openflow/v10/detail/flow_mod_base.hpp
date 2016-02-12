#ifndef CANARD_NETWORK_OPENFLOW_V10_FLOW_MOD_BASE_HPP
#define CANARD_NETWORK_OPENFLOW_V10_FLOW_MOD_BASE_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace flow_mod_detail {

    template <class FlowModType>
    class flow_mod_base
        : public v10_detail::basic_openflow_message<FlowModType>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_FLOW_MOD;

        auto header() const noexcept
            -> v10_detail::ofp_header const&
        {
            return flow_mod_.header;
        }

        auto buffer_id() const noexcept
            -> std::uint32_t
        {
            return flow_mod_.buffer_id;
        }

        auto actions() const noexcept
            -> action_list const&
        {
            return actions_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_mod_);
            return actions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> FlowModType
        {
            auto const flow_mod
                = detail::decode<v10_detail::ofp_flow_mod>(first, last);
            last = std::next(
                    first
                  , flow_mod.header.length - sizeof(v10_detail::ofp_flow_mod));
            auto actions = action_list::decode(first, last);
            return FlowModType{flow_mod, std::move(actions)};
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < sizeof(v10_detail::ofp_flow_mod)) {
                throw std::runtime_error{"invalid length"};
            }
        }

    protected:
        flow_mod_base(
                  match_set const& match
                , std::uint16_t const priority
                , std::uint64_t const cookie
                , action_list&& actions
                , std::uint16_t const idle_timeout
                , std::uint16_t const hard_timeout
                , std::uint16_t const flags
                , std::uint32_t const buffer_id
                , std::uint32_t const xid)
            : flow_mod_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(
                              sizeof(v10_detail::ofp_flow_mod)
                            + actions.length())
                    , xid
                  }
                , match.ofp_match()
                , cookie
                , FlowModType::command_type
                , idle_timeout
                , hard_timeout
                , priority
                , buffer_id
                , 0
                , flags
              }
            , actions_(std::move(actions))
        {
        }

        flow_mod_base(flow_mod_base const& other) = default;

        flow_mod_base(flow_mod_base&& other)
            : flow_mod_(other.flow_mod_)
            , actions_(std::move(other).actions_)
        {
            other.flow_mod_.header.length = sizeof(v10_detail::ofp_flow_mod);
        }

        auto operator=(flow_mod_base const& other)
            -> flow_mod_base& = default;

        auto operator=(flow_mod_base&& other)
            -> flow_mod_base&
        {
            auto tmp = std::move(other);
            std::swap(flow_mod_, tmp.flow_mod_);
            actions_.swap(tmp.actions_);
            return *this;
        }

        flow_mod_base(
                  v10_detail::ofp_flow_mod const& flow_mod
                , action_list&& actions)
            : flow_mod_(flow_mod)
            , actions_(std::move(actions))
        {
        }

        auto ofp_flow_mod() const noexcept
            -> v10_detail::ofp_flow_mod const&
        {
            return flow_mod_;
        }

    private:
        v10_detail::ofp_flow_mod flow_mod_;
        action_list actions_;
    };

} // namespace flow_mod_detail
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_FLOW_MOD_BASE_HPP
