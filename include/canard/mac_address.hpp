#ifndef CANARD_MAC_ADDRESS_HPP
#define CANARD_MAC_ADDRESS_HPP

#include <cstring>
#include <array>
#include <net/ethernet.h>
#include <boost/format.hpp>
#include <boost/operators.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace canard {

    class mac_address
        : boost::less_than_comparable<mac_address, boost::equality_comparable<mac_address>>
    {
    public:
        using bytes_type = std::array<unsigned char, 6>;

        mac_address()
            : addr_{}
        {
        }

        explicit mac_address(bytes_type const& bytes)
        {
            std::memcpy(addr_.octet, bytes.data(), bytes.size());
        }

        explicit mac_address(unsigned char (&array)[6])
        {
            std::memcpy(addr_.octet, array, sizeof(array));
        }

        template <class ByteRange>
        explicit mac_address(ByteRange const& range)
        {
            boost::copy(range, addr_.octet);
        }

        auto to_bytes() const
            -> bytes_type
        {
            auto bytes = bytes_type{};
            std::memcpy(bytes.data(), addr_.octet, bytes.size());
            return bytes;
        }

        auto to_string() const
            -> std::string
        {
            return (boost::format{"%x:%x:%x:%x:%x:%x"}
                % std::uint16_t{addr_.octet[0]}
                % std::uint16_t{addr_.octet[1]}
                % std::uint16_t{addr_.octet[2]}
                % std::uint16_t{addr_.octet[3]}
                % std::uint16_t{addr_.octet[4]}
                % std::uint16_t{addr_.octet[5]})
                .str();
        }

        static auto broadcast()
            -> mac_address
        {
            return mac_address(bytes_type{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});
        }

        friend auto operator==(mac_address const& lhs, mac_address const& rhs)
            -> bool
        {
            return std::memcmp(&lhs.addr_, &rhs.addr_, sizeof(ether_addr)) == 0;
        }

        friend auto operator<(mac_address const& lhs, mac_address const& rhs)
            -> bool
        {
            return std::memcmp(&lhs.addr_, &rhs.addr_, sizeof(ether_addr)) < 0;
        }

    private:
        ether_addr addr_;
    };

    template <class OStream>
    inline auto operator<<(OStream& os, mac_address const& mac)
        -> OStream&
    {
        return os << mac.to_string();
    }

} // namespace canard

#endif // CANARD_MAC_ADDRESS_HPP
