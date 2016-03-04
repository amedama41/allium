#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_BASE_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_BASE_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace flow_mod_detail {

    template <class FlowModType>
    class flow_mod_base
        : public v13_detail::basic_openflow_message<FlowModType>
    {
        static constexpr std::size_t base_flow_mod_size
            = sizeof(v13_detail::ofp_flow_mod) + sizeof(v13_detail::ofp_match);

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

        auto instructions() const noexcept
            -> instruction_set const&
        {
            return instructions_;
        }

        auto buffer_id() const noexcept
            -> std::uint32_t
        {
            return flow_mod_.buffer_id;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, flow_mod_);
            match_.encode(container);
            return instructions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> FlowModType
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
                    < v13_detail::exact_length(ofp_match.length)) {
                throw std::runtime_error{"oxm_match length is too big"};
            }
            auto match = oxm_match_set::decode(first, last);

            auto instructions = instruction_set::decode(first, last);

            return FlowModType{
                flow_mod, std::move(match), std::move(instructions)
            };
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
        flow_mod_base(
                  oxm_match_set&& match
                , std::uint16_t const priority
                , std::uint64_t const cookie
                , std::uint64_t const cookie_mask
                , instruction_set&& instructions
                , std::uint8_t const table_id
                , std::uint16_t const idle_timeout
                , std::uint16_t const hard_timeout
                , std::uint16_t const flags
                , std::uint32_t const buffer_id
                , std::uint32_t const xid)
            : flow_mod_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , std::uint16_t(
                              sizeof(v13_detail::ofp_flow_mod)
                            + v13_detail::exact_length(match.length())
                            + instructions.length())
                    , xid
                  }
                , cookie
                , cookie_mask
                , table_id
                , FlowModType::command_type
                , idle_timeout
                , hard_timeout
                , priority
                , buffer_id
                , 0
                , 0
                , flags
                , { 0, 0 }
              }
            , match_(std::move(match))
            , instructions_(std::move(instructions))
        {
        }

        flow_mod_base(
                 v13_detail::ofp_flow_mod const& flow_mod
               , oxm_match_set&& match
               , instruction_set&& instructions)
            : flow_mod_(flow_mod)
            , match_(std::move(match))
            , instructions_(std::move(instructions))
        {
        }

        flow_mod_base(flow_mod_base const&) = default;

        flow_mod_base(flow_mod_base&& other)
            : flow_mod_(other.flow_mod_)
            , match_(std::move(other).match_)
            , instructions_(std::move(other).instructions_)
        {
            other.flow_mod_.header.length = base_flow_mod_size;
        }

        auto operator=(flow_mod_base const&)
            -> flow_mod_base& = default;

        auto operator=(flow_mod_base&& other)
            -> flow_mod_base&
        {
            auto tmp = std::move(other);
            std::swap(flow_mod_, tmp.flow_mod_);
            match_.swap(tmp.match_);
            instructions_.swap(tmp.instructions_);
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
        instruction_set instructions_;
    };

} // namespace flow_mod_detail
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_BASE_HPP
