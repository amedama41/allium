#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <canard/constant_range.hpp>
#include <canard/type_traits.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/detail/oxm_match_field_set.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class oxm_match_set
    {
    public:
        static constexpr protocol::ofp_match_type match_type
            = protocol::OFPMT_OXM;
        static constexpr std::uint16_t match_base_length
            = offsetof(v13_detail::ofp_match, pad);

        template <
              class... OXMMatchFields
            , typename std::enable_if<
                    !is_related<oxm_match_set, OXMMatchFields...>::value
              >::type* = nullptr
        >
        oxm_match_set(OXMMatchFields&&... oxm_fields)
            : oxm_match_fields_{std::forward<OXMMatchFields>(oxm_fields)...}
            , length_(match_base_length + oxm_match_fields_.length())
        {
        }

        oxm_match_set(oxm_match_set const&) = default;

        oxm_match_set(oxm_match_set&& other)
            : oxm_match_fields_(std::move(other.oxm_match_fields_))
            , length_(other.length_)
        {
            other.length_ = match_base_length;
        }

        auto operator=(oxm_match_set const&)
            -> oxm_match_set& = default;

        auto operator=(oxm_match_set&& other)
            -> oxm_match_set&
        {
            auto match = std::move(other);
            this->swap(match);
            return *this;
        }

        void swap(oxm_match_set& other)
        {
            oxm_match_fields_.swap(other.oxm_match_fields_);
            std::swap(length_, other.length_);
        }

        auto type() const noexcept
            -> protocol::ofp_match_type
        {
            return match_type;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return length_;
        }

        template <class OXMMatchField>
        void add(OXMMatchField&& field)
        {
            auto const field_length = field.length();
            length_ -= oxm_match_fields_.add(
                    std::forward<OXMMatchField>(field));
            length_ += field_length;
        }

        template <class OXMMatchField>
        auto get() const
            -> boost::optional<OXMMatchField const&>
        {
            return oxm_match_fields_.get<OXMMatchField>();
        }

        auto operator[](std::uint32_t const oxm_type) const
            -> boost::optional<any_oxm_match_field const&>
        {
            return oxm_match_fields_[oxm_type];
        }

        auto begin() const noexcept
            -> decltype(v13_detail::oxm_match_field_set{}.begin())
        {
            return oxm_match_fields_.begin();
        }

        auto end() const noexcept
            -> decltype(v13_detail::oxm_match_field_set{}.end())
        {
            return oxm_match_fields_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t{match_type});
            detail::encode(container, length());
            oxm_match_fields_.encode(container);
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_match_set
        {
            std::advance(first, sizeof(std::uint16_t));
            auto const length = detail::decode<std::uint16_t>(first, last);
            last = std::next(first, length - match_base_length);
            auto oxm_fields
                = v13_detail::oxm_match_field_set::decode(first, last);
            std::advance(first, v13_detail::padding_length(length));
            return oxm_match_set{length, std::move(oxm_fields)};
        }

        static void validate(v13_detail::ofp_match const& match)
        {
            if (match.type != protocol::OFPMT_OXM) {
                throw std::runtime_error{"match_type is not OFPMT_OXM"};
            }
            if (match.length < offsetof(v13_detail::ofp_match, pad)) {
                throw std::runtime_error{"invalid oxm_match length"};
            }
        }

    private:
        oxm_match_set(std::uint16_t const length
                , v13_detail::oxm_match_field_set&& oxm_field)
            : oxm_match_fields_{std::move(oxm_field)}
            , length_(length)
        {
        }

    private:
        v13_detail::oxm_match_field_set oxm_match_fields_;
        std::uint16_t length_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_HPP
