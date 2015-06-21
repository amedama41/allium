#ifndef CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/flow_entry.hpp>
#include <canard/network/protocol/openflow/v13/instruction_set.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class flow_mod_add
        : public v13_detail::basic_openflow_message<flow_mod_add>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_ADD;

        flow_mod_add(flow_entry entry, std::uint8_t const table_id, std::uint16_t const flags)
            : flow_mod_add{
                std::move(entry), table_id, flags, protocol::OFP_NO_BUFFER
              }
        {
        }

        flow_mod_add(flow_entry entry, std::uint8_t const table_id
                , std::uint16_t const flags, std::uint32_t buffer_id)
            : mod_add_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , calc_length(entry), get_xid()
                  }
                , entry.cookie(), 0
                , table_id, command_type
                , entry.idle_timeout(), entry.hard_timeout()
                , entry.priority()
                , buffer_id, protocol::OFPP_ANY, protocol::OFPG_ANY
                , flags, {0, 0}
              }
            , entry_(std::move(entry))
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return mod_add_.header;
        }

        auto table_id() const
            -> std::uint8_t
        {
            return mod_add_.table_id;
        }

        auto flags() const
            -> std::uint16_t
        {
            return mod_add_.flags;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, mod_add_);
            entry_.match().encode(container);
            return entry_.instructions().encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_mod_add
        {
            auto const mod_add = detail::decode<v13_detail::ofp_flow_mod>(first, last);
            if (std::distance(first, last) != mod_add.header.length - sizeof(v13_detail::ofp_flow_mod)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            auto instructions = instruction_set::decode(first, last);
            return {mod_add, std::move(match), std::move(instructions)};
        }

    private:
        flow_mod_add(v13_detail::ofp_flow_mod const& mod_add, oxm_match match, instruction_set instructions)
            : mod_add_(mod_add)
            , entry_{
                  {std::move(match), mod_add.priority}
                , std::move(instructions)
                , {0, 0}
                , {mod_add.idle_timeout, mod_add.hard_timeout}
                , mod_add.cookie
              }
        {
        }

        static auto calc_length(flow_entry const& entry)
            -> std::uint16_t
        {
            return std::uint16_t{sizeof(v13_detail::ofp_flow_mod)}
                + detail::exact_length(entry.match().length()) + entry.instructions().length();
        }

    private:
        v13_detail::ofp_flow_mod mod_add_;
        flow_entry entry_;
    };

    class flow_mod_modify_strict
        : public v13_detail::basic_openflow_message<flow_mod_modify_strict>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_MODIFY_STRICT;

        template <class... Instructions>
        flow_mod_modify_strict(flow_entry_id entry_id, Instructions&&... instructions);

        flow_mod_modify_strict(flow_entry_id entry_id, std::uint8_t const table_id
                , std::uint64_t const cookie, std::uint64_t const cookie_mask
                , std::uint32_t const buffer_id
                , bool const is_reset_count
                , oxm_match match
                , instruction_set instructions)
            : mod_modify_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , message_type
                    , detail::exact_length(sizeof(v13_detail::ofp_flow_mod) + entry_id.match_.length() + instructions.length())
                    , get_xid()
                  }
                , cookie, cookie_mask
                , table_id, command_type
                , 0, 0
                , entry_id.priority_
                , buffer_id, protocol::OFPP_ANY, protocol::OFPG_ANY
                , std::uint16_t(is_reset_count ? protocol::OFPFF_RESET_COUNTS : 0)
                , {0, 0}
            }
            , match_(std::move(entry_id.match_))
            , instructions_(std::move(instructions))
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return mod_modify_.header;
        }

        using basic_openflow_message<flow_mod_modify_strict>::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, mod_modify_);
            match_.encode(container);
            return instructions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_mod_modify_strict
        {
            auto const mod_modify = detail::decode<v13_detail::ofp_flow_mod>(first, last);
            if (std::distance(first, last) != mod_modify.header.length - sizeof(v13_detail::ofp_flow_mod)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            auto instructions = instruction_set::decode(first, last);
            return flow_mod_modify_strict{mod_modify, std::move(match), std::move(instructions)};
        }

    private:
        flow_mod_modify_strict(v13_detail::ofp_flow_mod const& mod_modify, oxm_match match, instruction_set instructions)
            : mod_modify_(mod_modify)
            , match_(std::move(match))
            , instructions_(std::move(instructions))
        {
        }

    private:
        v13_detail::ofp_flow_mod mod_modify_;
        oxm_match match_;
        instruction_set instructions_;
    };


    class flow_mod_modify
        : public v13_detail::basic_openflow_message<flow_mod_modify>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_MODIFY;

        flow_mod_modify(oxm_match match, std::uint8_t const table_id
                , std::uint64_t const cookie, std::uint64_t const cookie_mask
                , std::uint16_t const flags, std::uint32_t const buffer_id
                , instruction_set instructions)
            : mod_modify_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , calc_length(match, instructions), get_xid()
                  }
                , cookie, cookie_mask
                , table_id, command_type
                , 0, 0
                , 0
                , buffer_id, protocol::OFPP_ANY, protocol::OFPG_ANY
                , std::uint16_t(flags & protocol::OFPFF_RESET_COUNTS), {0, 0}
              }
            , match_(std::move(match))
            , instructions_{std::move(instructions)}
        {
        }

        template <class... Instructions>
        flow_mod_modify(oxm_match match, std::uint8_t const table_id, Instructions&&... instructions)
            : flow_mod_modify{
                  std::move(match), table_id
                , std::uint64_t{0}, std::uint64_t{0}
                , std::uint16_t{0}, std::uint32_t{protocol::OFP_NO_BUFFER}
                , instruction_set{std::forward<Instructions>(instructions)...}
              }
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return mod_modify_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, mod_modify_);
            match_.encode(container);
            return instructions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_mod_modify
        {
            auto const mod_modify = detail::decode<v13_detail::ofp_flow_mod>(first, last);
            if (std::distance(first, last) != mod_modify.header.length - sizeof(v13_detail::ofp_flow_mod)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            auto instructions = instruction_set::decode(first, last);
            return flow_mod_modify{mod_modify, std::move(match), std::move(instructions)};
        }

    private:
        flow_mod_modify(v13_detail::ofp_flow_mod const& mod_modify, oxm_match match, instruction_set instructions)
            : mod_modify_(mod_modify)
            , match_(std::move(match))
            , instructions_(std::move(instructions))
        {
        }

    private:
        static auto calc_length(oxm_match const& match, instruction_set const& instructions)
            -> std::uint16_t
        {
            return std::uint16_t{sizeof(v13_detail::ofp_flow_mod)}
                + detail::exact_length(match.length()) + instructions.length();
        }

    private:
        v13_detail::ofp_flow_mod mod_modify_;
        oxm_match match_;
        instruction_set instructions_;
    };

    class flow_mod_delete_strict
        : public v13_detail::basic_openflow_message<flow_mod_delete_strict>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE_STRICT;

        explicit flow_mod_delete_strict(flow_entry_id entry_id)
            : flow_mod_delete_strict{
                  std::move(entry_id), protocol::OFPTT_ALL, 0, 0
                , protocol::OFPP_ANY, protocol::OFPG_ANY
              }
        {
        }

        flow_mod_delete_strict(flow_entry_id entry_id, std::uint8_t const table_id)
            : flow_mod_delete_strict{
                  std::move(entry_id), table_id, 0, 0
                , protocol::OFPP_ANY, protocol::OFPG_ANY
              }
        {
        }

        flow_mod_delete_strict(flow_entry_id entry_id, std::uint8_t const table_id
                , std::uint64_t const cookie, std::uint64_t const cookie_mask
                , std::uint32_t const out_port, std::uint32_t const out_group)
            : mod_delete_{
                  v13_detail::ofp_header{
                        protocol::OFP_VERSION, message_type
                      , detail::exact_length(sizeof(v13_detail::ofp_flow_mod) + entry_id.match_.length())
                      , get_xid()
                  }
                , cookie, cookie_mask
                , table_id
                , command_type
                , 0, 0
                , entry_id.priority_
                , 0
                , out_port
                , out_group
                , 0, {0, 0}
            }
            , match_(std::move(entry_id.match_))
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return mod_delete_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, mod_delete_);
            return match_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_mod_delete_strict
        {
            auto const mod_delete = detail::decode<v13_detail::ofp_flow_mod>(first, last);
            if (std::distance(first, last) != mod_delete.header.length - sizeof(v13_detail::ofp_flow_mod)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            return flow_mod_delete_strict{mod_delete, std::move(match)};
        }

    private:
        flow_mod_delete_strict(v13_detail::ofp_flow_mod const& mod_delete, oxm_match match)
            : mod_delete_(mod_delete)
            , match_(std::move(match))
        {
        }

    private:
        v13_detail::ofp_flow_mod mod_delete_;
        oxm_match match_;
    };

    class flow_mod_delete
        : public v13_detail::basic_openflow_message<flow_mod_delete>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_FLOW_MOD;
        static protocol::ofp_flow_mod_command const command_type
            = protocol::OFPFC_DELETE;

        explicit flow_mod_delete(oxm_match match)
            : flow_mod_delete{
                  std::move(match), protocol::OFPTT_ALL, 0, 0
                , protocol::OFPP_ANY, protocol::OFPG_ANY
              }
        {
        }

        flow_mod_delete(oxm_match match, std::uint8_t const table_id
                , std::uint64_t const cookie, std::uint64_t const cookie_mask
                , std::uint32_t const out_port, std::uint32_t const out_group)
            : mod_delete_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , detail::exact_length(sizeof(v13_detail::ofp_flow_mod) + match.length())
                    , get_xid()
                  }
                , 0, 0, table_id, command_type, 0, 0, 0, 0, out_port, out_group, 0, {0, 0}
              }
            , match_(std::move(match))
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return mod_delete_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, mod_delete_);
            return match_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> flow_mod_delete
        {
            auto const mod_delete = detail::decode<v13_detail::ofp_flow_mod>(first, last);
            if (std::distance(first, last) != mod_delete.header.length - sizeof(v13_detail::ofp_flow_mod)) {
                throw 2;
            }
            auto match = oxm_match::decode(first, last);
            return flow_mod_delete{mod_delete, std::move(match)};
        }

    private:
        flow_mod_delete(v13_detail::ofp_flow_mod const& mod_delete, oxm_match match)
            : mod_delete_(mod_delete)
            , match_(std::move(match))
        {
        }

    private:
        v13_detail::ofp_flow_mod mod_delete_;
        oxm_match match_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_FLOW_MOD_HPP
