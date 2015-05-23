#ifndef CANARD_MAC_ADDRESS_HPP
#define CANARD_MAC_ADDRESS_HPP

#include <cstring>
#include <array>
#include <boost/format.hpp>
#include <boost/operators.hpp>

namespace canard {

    class mac_address
        : boost::less_than_comparable<mac_address, boost::equality_comparable<mac_address>>
    {
    public:
        using bytes_type = std::array<std::uint8_t, 6>;

        mac_address()
            : addr_{}
        {
        }

        explicit mac_address(bytes_type const& bytes)
            : addr_(bytes)
        {
        }

        explicit mac_address(std::uint8_t const (&array)[6])
            : addr_{{array[0], array[1], array[2], array[3], array[4], array[5]}}
        {
        }

        auto to_bytes() const
            -> bytes_type const&
        {
            return addr_;
        }

        auto to_string() const
            -> std::string
        {
            return boost::str(boost::format{"%02x:%02x:%02x:%02x:%02x:%02x"}
                % std::uint16_t{addr_[0]}
                % std::uint16_t{addr_[1]}
                % std::uint16_t{addr_[2]}
                % std::uint16_t{addr_[3]}
                % std::uint16_t{addr_[4]}
                % std::uint16_t{addr_[5]});
        }

        static auto broadcast()
            -> mac_address
        {
            return mac_address({{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});
        }

        friend auto operator==(mac_address const& lhs, mac_address const& rhs)
            -> bool
        {
            return lhs.addr_ == rhs.addr_;
        }

        friend auto operator<(mac_address const& lhs, mac_address const& rhs)
            -> bool
        {
            return lhs.addr_ < rhs.addr_;
        }

    private:
        bytes_type addr_;
    };

    template <class OStream>
    inline auto operator<<(OStream& os, mac_address const& mac)
        -> OStream&
    {
        return os << mac.to_string();
    }

} // namespace canard

#endif // CANARD_MAC_ADDRESS_HPP
