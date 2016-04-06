#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP

#include <cstdint>
#include <boost/operators.hpp>
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
        : private boost::equality_comparable<oxm_id>
    {
    public:
        explicit oxm_id(std::uint32_t const oxm_header) noexcept
            : oxm_header_{oxm_header}
        {
        }

        oxm_id(protocol::ofp_oxm_class const oxm_class
             , std::uint8_t const field
             , bool const hasmask
             , std::uint8_t const length) noexcept
            : oxm_header_{
                  (std::uint32_t(oxm_class) << 16)
                | (std::uint32_t{field} << 9)
                | (std::uint32_t{hasmask} << 8)
                | length
              }
        {
        }

        auto oxm_class() const noexcept
            -> protocol::ofp_oxm_class
        {
            return protocol::ofp_oxm_class(oxm_header_ >> 16);
        }

        auto oxm_field() const noexcept
            -> std::uint8_t
        {
            return (oxm_header_ >> 9) & 0x7f;
        }

        auto oxm_type() const noexcept
            -> std::uint32_t
        {
            return (oxm_header_ >> 9);
        }

        auto oxm_has_mask() const noexcept
            -> bool
        {
            return oxm_header_ & 0x100;
        }

        auto oxm_length() const noexcept
            -> std::uint8_t
        {
            return oxm_header_ & 0xff;
        }

        auto oxm_header() const noexcept
            -> std::uint32_t
        {
            return oxm_header_;
        }

        static constexpr auto length() noexcept
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
            return oxm_id{detail::decode<std::uint32_t>(first, last)};
        }

    private:
        std::uint32_t oxm_header_;
    };

    inline auto operator==(oxm_id const& lhs, oxm_id const& rhs) noexcept
        -> bool
    {
        return lhs.oxm_header() == rhs.oxm_header();
    }


    class oxm_experimenter_id
        : private boost::equality_comparable<oxm_experimenter_id>
    {
        using raw_ofp_type = v13_detail::ofp_oxm_experimenter_header;

    public:
        oxm_experimenter_id(
                  std::uint8_t const oxm_field
                , bool const oxm_has_mask
                , std::uint8_t const oxm_length
                , std::uint32_t const experimenter) noexcept
            : oxm_experimenter_header_{
                  (std::uint32_t(protocol::OFPXMC_EXPERIMENTER) << 16)
                | (std::uint32_t(oxm_field) << 9)
                | (std::uint32_t(oxm_has_mask) << 8)
                | (std::uint32_t(oxm_length))
                , experimenter
              }
        {
        }

        static constexpr auto oxm_class() noexcept
            -> protocol::ofp_oxm_class
        {
            return protocol::OFPXMC_EXPERIMENTER;
        }

        auto oxm_field() const noexcept
            -> std::uint8_t
        {
            return (oxm_experimenter_header_.oxm_header >> 9) & 0x7f;
        }

        auto oxm_type() const noexcept
            -> std::uint32_t
        {
            return (oxm_experimenter_header_.oxm_header >> 9);
        }

        auto oxm_has_mask() const noexcept
            -> bool
        {
            return oxm_experimenter_header_.oxm_header & 0x100;
        }

        auto oxm_length() const noexcept
            -> std::uint8_t
        {
            return oxm_experimenter_header_.oxm_header & 0xff;
        }

        auto oxm_header() const noexcept
            -> std::uint32_t
        {
            return oxm_experimenter_header_.oxm_header;
        }

        auto experimenter() const noexcept
            -> std::uint32_t
        {
            return oxm_experimenter_header_.experimenter;
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(raw_ofp_type);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, oxm_experimenter_header_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_experimenter_id
        {
            return oxm_experimenter_id{
                detail::decode<raw_ofp_type>(first, last)
            };
        }

    private:
        explicit oxm_experimenter_id(
                raw_ofp_type const& oxm_experimenter_header) noexcept
            : oxm_experimenter_header_(oxm_experimenter_header)
        {
        }

    private:
        raw_ofp_type oxm_experimenter_header_;
    };

    inline auto operator==(
              oxm_experimenter_id const& lhs
            , oxm_experimenter_id const& rhs) noexcept
        -> bool
    {
        return lhs.oxm_header() == rhs.oxm_header()
            && lhs.experimenter() == rhs.experimenter();
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_ID_HPP
