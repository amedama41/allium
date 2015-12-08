#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP

#include <cstdint>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_id.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class oxm_id
    {
    public:
        explicit oxm_id(std::uint32_t const oxm_header)
            : oxm_header_{oxm_header}
        {
        }

        oxm_id(protocol::ofp_oxm_class const oxm_class, std::uint8_t const field
                , bool const hasmask, std::uint8_t const length)
            : oxm_header_{(std::uint32_t(oxm_class) << 16) | (std::uint32_t{field} << 9) | (std::uint32_t{hasmask} << 8) | length}
        {
        }

        auto oxm_class() const
            -> protocol::ofp_oxm_class
        {
            return protocol::ofp_oxm_class(oxm_header_ >> 16);
        }

        auto oxm_field() const
            -> std::uint8_t
        {
            return (oxm_header_ >> 9) & 0x7f;
        }

        auto oxm_type() const
            -> std::uint32_t
        {
            return (oxm_header_ >> 9);
        }

        auto oxm_has_mask() const
            -> bool
        {
            return oxm_header_ & 0x100;
        }

        auto oxm_length() const
            -> std::uint8_t
        {
            return oxm_header_ & 0xff;
        }

        auto oxm_header() const
            -> std::uint32_t
        {
            return oxm_header_;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(std::uint32_t);
        };

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, oxm_header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_id
        {
            auto const oxm_header = detail::decode<std::uint32_t>(first, last);
            return oxm_id{oxm_header};
        }

    private:
        std::uint32_t oxm_header_;
    };

    class oxm_experimenter_id
    {
    public:
        oxm_experimenter_id(std::uint32_t const oxm_header, std::uint32_t const experimenter)
            : header_{oxm_header, experimenter}
        {
        }

        auto oxm_class() const
            -> protocol::ofp_oxm_class
        {
            return protocol::ofp_oxm_class(header_.oxm_header >> 16);
        }

        auto oxm_field() const
            -> std::uint8_t
        {
            return (header_.oxm_header >> 9) & 0x7f;
        }

        auto oxm_type() const
            -> std::uint32_t
        {
            return (header_.oxm_header >> 9);
        }

        auto oxm_has_mask() const
            -> bool
        {
            return header_.oxm_header & 0x100;
        }

        auto oxm_length() const
            -> std::uint8_t
        {
            return header_.oxm_header & 0xff;
        }

        auto oxm_header() const
            -> std::uint32_t
        {
            return header_.oxm_header;
        }

        auto experimenter() const
            -> std::uint32_t
        {
            return header_.experimenter;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(v13_detail::ofp_oxm_experimenter_header);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_experimenter_id
        {
            auto const header = detail::decode<v13_detail::ofp_oxm_experimenter_header>(first, last);
            return oxm_experimenter_id{header.oxm_header, header.experimenter};
        }

    private:
        v13_detail::ofp_oxm_experimenter_header header_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP
