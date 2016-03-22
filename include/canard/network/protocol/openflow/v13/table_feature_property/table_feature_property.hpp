#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP

#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>
#include <boost/format.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/variant/variant.hpp>
#include <canard/constant_range.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/action_id.hpp>
#include <canard/network/protocol/openflow/v13/any_action_id.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction_id.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_id.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/instruction_id.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_id.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace table_feature_properties {

        namespace table_feature_properties_detail {

            template <class T>
            class basic_prop_instructions
            {
                using instruction_id_list = std::vector<any_instruction_id>;

            public:
                using value_type = instruction_id_list::value_type;
                using reference = instruction_id_list::const_reference;
                using iterator = instruction_id_list::const_iterator;
                using const_iterator = instruction_id_list::const_iterator;

            protected:
                explicit basic_prop_instructions(instruction_id_list instruction_ids)
                    : instruction_ids_(std::move(instruction_ids))
                {
                }

                basic_prop_instructions(basic_prop_instructions const&) = default;
                basic_prop_instructions(basic_prop_instructions&&) noexcept = default;

                auto operator=(basic_prop_instructions&&) noexcept
                    -> basic_prop_instructions& = default;

                auto operator=(basic_prop_instructions const& other)
                    -> basic_prop_instructions&
                {
                    auto tmp = other;
                    swap(tmp);
                    return *this;
                }

            public:
                void swap(basic_prop_instructions& other)
                {
                    instruction_ids_.swap(other.instruction_ids_);
                }

                auto type() const
                    -> protocol::ofp_table_feature_prop_type
                {
                    return T::prop_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    using boost::adaptors::transformed;
                    return boost::accumulate(instruction_ids_ | transformed([](any_instruction_id const& id) {
                            return id.length();
                    }), std::uint16_t{sizeof(v13_detail::ofp_table_feature_prop_instructions)});
                }

                auto begin() const
                    -> const_iterator
                {
                    return instruction_ids_.begin();
                }

                auto end() const
                    -> const_iterator
                {
                    return instruction_ids_.end();
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    detail::encode(container, v13_detail::ofp_table_feature_prop_instructions{std::uint16_t(type()), length()});
                    boost::for_each(instruction_ids_, [&](any_instruction_id const& id) {
                        id.encode(container);
                    });
                    return detail::encode_byte_array(
                              container
                            , detail::padding
                            , v13_detail::padding_length(length()));
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const prop_instructions = detail::decode<v13_detail::ofp_table_feature_prop_instructions>(first, last);
                    if (std::distance(first, last) < prop_instructions.length - sizeof(v13_detail::ofp_table_feature_prop_instructions)) {
                        throw 2;
                    }
                    last = std::next(first, prop_instructions.length - sizeof(v13_detail::ofp_table_feature_prop_instructions));
                    auto instruction_ids = instruction_id_list{};
                    instruction_ids.reserve(std::distance(first, last) / sizeof(std::uint32_t));
                    while (first != last) {
                        instruction_ids.push_back(any_instruction_id::decode(first, last));
                    }
                    std::advance(first, v13_detail::padding_length(prop_instructions.length));
                    return T{std::move(instruction_ids)};
                }

            private:
                std::vector<any_instruction_id> instruction_ids_;
            };

            template <class T>
            class basic_prop_next_tables
            {
                using table_id_list = std::vector<std::uint8_t>;

            public:
                using value_type = table_id_list::value_type;
                using reference = table_id_list::const_reference;
                using iterator = table_id_list::const_iterator;
                using const_iterator = table_id_list::const_iterator;

            protected:
                explicit basic_prop_next_tables(table_id_list table_ids)
                    : table_ids_(std::move(table_ids))
                {
                }

                basic_prop_next_tables(basic_prop_next_tables const&) = default;
                basic_prop_next_tables(basic_prop_next_tables&&) noexcept = default;

                auto operator=(basic_prop_next_tables&&) noexcept
                    -> basic_prop_next_tables& = default;

                auto operator=(basic_prop_next_tables const& other)
                    -> basic_prop_next_tables&
                {
                    auto tmp = other;
                    swap(tmp);
                    return *this;
                }

            public:
                void swap(basic_prop_next_tables& other)
                {
                    table_ids_.swap(other.table_ids_);
                }

                auto type() const
                    -> protocol::ofp_table_feature_prop_type
                {
                    return T::prop_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    return sizeof(v13_detail::ofp_table_feature_prop_next_tables)
                        + table_ids_.size() * sizeof(value_type);
                }

                auto begin() const
                    -> const_iterator
                {
                    return table_ids_.begin();
                }

                auto end() const
                    -> const_iterator
                {
                    return table_ids_.end();
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    detail::encode(container, v13_detail::ofp_table_feature_prop_next_tables{std::uint16_t(type()), length()});
                    detail::encode_byte_array(
                            container, table_ids_.data(), table_ids_.size());
                    return detail::encode_byte_array(
                              container
                            , detail::padding
                            , v13_detail::padding_length(length()));
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const prop_next_tables = detail::decode<v13_detail::ofp_table_feature_prop_next_tables>(first, last);
                    if (std::distance(first, last) < prop_next_tables.length - sizeof(v13_detail::ofp_table_feature_prop_next_tables)) {
                        throw 2;
                    }
                    last = std::next(first, prop_next_tables.length - sizeof(v13_detail::ofp_table_feature_prop_next_tables));
                    auto table_ids = table_id_list(first, last);
                    std::advance(first, table_ids.size());
                    std::advance(first, v13_detail::padding_length(prop_next_tables.length));
                    return T{std::move(table_ids)};
                }

            private:
                table_id_list table_ids_;
            };

            template <class T>
            class basic_prop_actions
            {
                using action_id_list = std::vector<any_action_id>;

            public:
                using value_type = action_id_list::value_type;
                using reference = action_id_list::const_reference;
                using const_reference = action_id_list::const_reference;
                using iterator = action_id_list::const_iterator;
                using const_iterator = action_id_list::const_iterator;

            protected:
                explicit basic_prop_actions(action_id_list action_ids)
                    : action_ids_(std::move(action_ids))
                {
                }

                basic_prop_actions(basic_prop_actions const&) = default;
                basic_prop_actions(basic_prop_actions&&) noexcept = default;

                auto operator=(basic_prop_actions&&) noexcept
                    -> basic_prop_actions& = default;

                auto operator=(basic_prop_actions const& other)
                    -> basic_prop_actions&
                {
                    auto tmp = other;
                    swap(tmp);
                    return *this;
                }

            public:
                void swap(basic_prop_actions& other)
                {
                    action_ids_.swap(other.action_ids_);
                }

                auto type() const
                    -> protocol::ofp_table_feature_prop_type
                {
                    return T::prop_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    using boost::adaptors::transformed;
                    return boost::accumulate(action_ids_ | transformed([](any_action_id const& id) {
                            return id.length();
                    }), std::uint16_t{sizeof(v13_detail::ofp_table_feature_prop_actions)});
                }

                auto begin() const
                    -> const_iterator
                {
                    return action_ids_.begin();
                }

                auto end() const
                    -> const_iterator
                {
                    return action_ids_.end();
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    detail::encode(container, v13_detail::ofp_table_feature_prop_actions{std::uint16_t(type()), length()});
                    boost::for_each(action_ids_, [&](any_action_id const& id) {
                        id.encode(container);
                    });
                    return detail::encode_byte_array(
                              container
                            , detail::padding
                            , v13_detail::padding_length(length()));
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const prop_actions = detail::decode<v13_detail::ofp_table_feature_prop_actions>(first, last);
                    if (std::distance(first, last) < prop_actions.length - sizeof(v13_detail::ofp_table_feature_prop_actions)) {
                        throw 2;
                    }
                    last = std::next(first, prop_actions.length - sizeof(v13_detail::ofp_table_feature_prop_actions));
                    auto action_ids = action_id_list{};
                    action_ids.reserve(std::distance(first, last) / sizeof(std::uint32_t));
                    while (first != last) {
                        action_ids.push_back(any_action_id::decode(first, last));
                    }
                    std::advance(first, v13_detail::padding_length(prop_actions.length));
                    return T{std::move(action_ids)};
                }

            private:
                action_id_list action_ids_;
            };

            template <class T>
            class basic_prop_oxm
            {
                using oxm_id_list = std::vector<any_oxm_id>;

            public:
                using value_type = oxm_id_list::value_type;
                using reference = oxm_id_list::const_reference;
                using const_reference = oxm_id_list::const_reference;
                using iterator = oxm_id_list::const_iterator;
                using const_iterator = oxm_id_list::const_iterator;

            protected:
                explicit basic_prop_oxm(oxm_id_list oxm_ids)
                    : oxm_ids_(std::move(oxm_ids))
                {
                }

                basic_prop_oxm(basic_prop_oxm const&) = default;
                basic_prop_oxm(basic_prop_oxm&&) noexcept = default;

                auto operator=(basic_prop_oxm&&) noexcept
                    -> basic_prop_oxm& = default;

                auto operator=(basic_prop_oxm const& other)
                    -> basic_prop_oxm&
                {
                    auto tmp = other;
                    swap(tmp);
                    return *this;
                }

            public:
                void swap(basic_prop_oxm& other)
                {
                    oxm_ids_.swap(other.oxm_ids_);
                }

                auto type() const
                    -> protocol::ofp_table_feature_prop_type
                {
                    return T::prop_type;
                }

                auto length() const
                    -> std::uint16_t
                {
                    using boost::adaptors::transformed;
                    return boost::accumulate(oxm_ids_ | transformed([](any_oxm_id const& id) {
                            return id.length();
                    }), std::uint16_t{sizeof(v13_detail::ofp_table_feature_prop_oxm)});
                }

                auto begin() const
                    -> const_iterator
                {
                    return oxm_ids_.begin();
                }

                auto end() const
                    -> const_iterator
                {
                    return oxm_ids_.end();
                }

                template <class Container>
                auto encode(Container& container) const
                    -> Container&
                {
                    detail::encode(container, v13_detail::ofp_table_feature_prop_oxm{std::uint16_t(type()), length()});
                    boost::for_each(oxm_ids_, [&](any_oxm_id const& id) {
                        id.encode(container);
                    });
                    return detail::encode_byte_array(
                              container
                            , detail::padding
                            , v13_detail::padding_length(length()));
                }

                template <class Iterator>
                static auto decode(Iterator& first, Iterator last)
                    -> T
                {
                    auto const prop_oxm = detail::decode<v13_detail::ofp_table_feature_prop_oxm>(first, last);
                    if (std::distance(first, last) < prop_oxm.length - sizeof(v13_detail::ofp_table_feature_prop_oxm)) {
                        throw 2;
                    }
                    last = std::next(first, prop_oxm.length - sizeof(v13_detail::ofp_table_feature_prop_oxm));
                    auto oxm_ids = oxm_id_list{};
                    oxm_ids.reserve(std::distance(first, last) / sizeof(std::uint32_t));
                    while (first != last) {
                        oxm_ids.push_back(any_oxm_id::decode(first, last));
                    }
                    std::advance(first, v13_detail::padding_length(prop_oxm.length));
                    return T{std::move(oxm_ids)};
                }

            private:
                oxm_id_list oxm_ids_;
            };

        } // namespace table_feature_properties_detail


        class prop_instructions
            : public table_feature_properties_detail::basic_prop_instructions<prop_instructions>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_INSTRUCTIONS;

            explicit prop_instructions(std::vector<any_instruction_id> instruction_ids)
                : basic_prop_instructions{std::move(instruction_ids)}
            {
            }

            template <class... InstructionIds>
            prop_instructions(InstructionIds&&... instruction_ids)
                : basic_prop_instructions{{std::forward<InstructionIds>(instruction_ids)...}}
            {
            }
        };

        class prop_instructions_miss
            : public table_feature_properties_detail::basic_prop_instructions<prop_instructions_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_INSTRUCTIONS_MISS;

            explicit prop_instructions_miss(std::vector<any_instruction_id> instruction_ids)
                : basic_prop_instructions{std::move(instruction_ids)}
            {
            }

            template <class... InstructionIds>
            prop_instructions_miss(InstructionIds&&... instruction_ids)
                : basic_prop_instructions{{std::forward<InstructionIds>(instruction_ids)...}}
            {
            }
        };

        class prop_next_tables
            : public table_feature_properties_detail::basic_prop_next_tables<prop_next_tables>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_NEXT_TABLES;

            explicit prop_next_tables(std::vector<std::uint8_t> table_ids)
                : basic_prop_next_tables{std::move(table_ids)}
            {
            }

            prop_next_tables(std::initializer_list<std::uint8_t> table_ids)
                : basic_prop_next_tables{table_ids}
            {
            }
        };

        class prop_next_tables_miss
            : public table_feature_properties_detail::basic_prop_next_tables<prop_next_tables_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_NEXT_TABLES_MISS;

            explicit prop_next_tables_miss(std::vector<std::uint8_t> table_ids)
                : basic_prop_next_tables{std::move(table_ids)}
            {
            }

            prop_next_tables_miss(std::initializer_list<std::uint8_t> table_ids)
                : basic_prop_next_tables{table_ids}
            {
            }
        };

        class prop_write_actions
            : public table_feature_properties_detail::basic_prop_actions<prop_write_actions>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_ACTIONS;

            explicit prop_write_actions(std::vector<any_action_id> action_ids)
                : basic_prop_actions{std::move(action_ids)}
            {
            }

            template <class... ActionIds>
            prop_write_actions(ActionIds&&... action_ids)
                : basic_prop_actions{{std::forward<ActionIds>(action_ids)...}}
            {
            }
        };

        class prop_write_actions_miss
            : public table_feature_properties_detail::basic_prop_actions<prop_write_actions_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_ACTIONS_MISS;

            explicit prop_write_actions_miss(std::vector<any_action_id> action_ids)
                : basic_prop_actions{std::move(action_ids)}
            {
            }

            template <class... ActionIds>
            prop_write_actions_miss(ActionIds&&... action_ids)
                : basic_prop_actions{{std::forward<ActionIds>(action_ids)...}}
            {
            }
        };

        class prop_apply_actions
            : public table_feature_properties_detail::basic_prop_actions<prop_apply_actions>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_ACTIONS;

            explicit prop_apply_actions(std::vector<any_action_id> action_ids)
                : basic_prop_actions{std::move(action_ids)}
            {
            }

            template <class... ActionIds>
            prop_apply_actions(ActionIds&&... action_ids)
                : basic_prop_actions{{std::forward<ActionIds>(action_ids)...}}
            {
            }
        };

        class prop_apply_actions_miss
            : public table_feature_properties_detail::basic_prop_actions<prop_apply_actions_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_ACTIONS_MISS;

            explicit prop_apply_actions_miss(std::vector<any_action_id> action_ids)
                : basic_prop_actions{std::move(action_ids)}
            {
            }

            template <class... ActionIds>
            prop_apply_actions_miss(ActionIds&&... action_ids)
                : basic_prop_actions{{std::forward<ActionIds>(action_ids)...}}
            {
            }
        };

        class prop_match
            : public table_feature_properties_detail::basic_prop_oxm<prop_match>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_MATCH;

            explicit prop_match(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_match(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        class prop_wildcards
            : public table_feature_properties_detail::basic_prop_oxm<prop_wildcards>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WILDCARDS;

            explicit prop_wildcards(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_wildcards(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        class prop_write_setfield
            : public table_feature_properties_detail::basic_prop_oxm<prop_write_setfield>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_SETFIELD;

            explicit prop_write_setfield(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_write_setfield(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        class prop_write_setfield_miss
            : public table_feature_properties_detail::basic_prop_oxm<prop_write_setfield_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_WRITE_SETFIELD_MISS;

            explicit prop_write_setfield_miss(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_write_setfield_miss(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        class prop_apply_setfield
            : public table_feature_properties_detail::basic_prop_oxm<prop_apply_setfield>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_SETFIELD;

            explicit prop_apply_setfield(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_apply_setfield(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        class prop_apply_setfield_miss
            : public table_feature_properties_detail::basic_prop_oxm<prop_apply_setfield_miss>
        {
        public:
            static protocol::ofp_table_feature_prop_type const prop_type
                = protocol::OFPTFPT_APPLY_SETFIELD_MISS;

            explicit prop_apply_setfield_miss(std::vector<any_oxm_id> oxm_ids)
                : basic_prop_oxm{std::move(oxm_ids)}
            {
            }

            template <class... OXMIds>
            prop_apply_setfield_miss(OXMIds&&... oxm_ids)
                : basic_prop_oxm{{std::forward<OXMIds>(oxm_ids)...}}
            {
            }
        };

        using property_list = std::tuple<
              prop_instructions, prop_instructions_miss
            , prop_next_tables, prop_next_tables_miss
            , prop_write_actions, prop_write_actions_miss, prop_apply_actions, prop_apply_actions_miss
            , prop_match, prop_wildcards
            , prop_write_setfield, prop_write_setfield_miss, prop_apply_setfield, prop_apply_setfield_miss
        >;
        using variant = boost::make_variant_over<property_list>::type;

        template <class ReturnType, class Iterator, class Function>
        inline auto decode(Iterator& first, Iterator last, Function&& function)
            -> ReturnType
        {
            auto copy_first = first;
            auto const header = detail::decode<v13_detail::ofp_table_feature_prop_header>(copy_first, last);
            switch (header.type) {
#           define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE(z, N, _) \
            case std::tuple_element<N, property_list>::type::prop_type: \
                return function(std::tuple_element<N, property_list>::type::decode(first, last));
            static_assert(std::tuple_size<property_list>::value == 14, "");
            BOOST_PP_REPEAT(14, CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE, _)
#           undef  CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_DECODE_CASE
            default:
                // TODO
                std::advance(first, v13_detail::exact_length(header.length));
                throw std::runtime_error{(boost::format{"%1%: prop_type(%2%) is unknwon"} % __func__ % header.type).str()};
            }
        }

        namespace table_feature_properties_detail {

            struct to_property_variant
            {
                template <class T>
                auto operator()(T&& t) const
                    -> variant
                {
                    return variant{std::forward<T>(t)};
                }
            };

        } // namespace table_feature_properties_detail

        template <class Iterator>
        inline auto decode(Iterator& first, Iterator last)
            -> variant
        {
            return decode<variant>(first, last, table_feature_properties_detail::to_property_variant{});
        }

    } // namespace table_feature_properties

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURE_PROPERTY_HPP
