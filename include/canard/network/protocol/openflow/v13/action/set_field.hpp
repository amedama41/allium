#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_FIELD_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <boost/format.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/constant_range.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/decode_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace actions {

        class set_field
        {
        public:
            static ofp_action_type const action_type = OFPAT_SET_FIELD;

            template <class OXMField>
            explicit set_field(OXMField&& oxm_field)
                : type_{action_type}
                , length_{detail::exact_length(sizeof(type_) + sizeof(length_) + oxm_field.length())}
                , field_{std::move(oxm_field)}
            {
            }

            auto type() const
                -> ofp_action_type
            {
                return action_type;
            }

            auto length() const
                -> std::uint16_t
            {
                return length_;
            }

            auto oxm_match_field() const
                -> any_oxm_match_field const&
            {
                return field_;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, type_);
                detail::encode(container, length_);
                field_.encode(container);
                return boost::push_back(container
                        , canard::make_constant_range(length_ - (sizeof(type_) + sizeof(length_) + field_.length()), 0));
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> set_field
            {
                auto const type = detail::decode<std::uint16_t>(first, last);
                if (type != action_type) {
                    throw 1;
                }
                auto const length = detail::decode<std::uint16_t>(first, last);
                if (length <= sizeof(detail::ofp_action_set_field)) {
                    throw 2;
                }
                auto field = decode_oxm_match_field(first, last);
                if (field.oxm_has_mask()) {
                    throw std::runtime_error{(boost::format{"%1%: field(%2%).oxm_has_mask is true"} % __func__ % field.oxm_type()).str()};
                }
                auto const field_length = field.length();
                if (length != detail::exact_length(sizeof(type_) + sizeof(length_) + field_length)) {
                    throw 2;
                }
                std::advance(first, length - (sizeof(type_) + sizeof(length_) + field_length));
                return set_field{std::move(field)};
            }

        private:
            uint16_t type_;
            uint16_t length_;
            any_oxm_match_field field_;
        };

    } // namespace actions

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_SET_FIELD_HPP
