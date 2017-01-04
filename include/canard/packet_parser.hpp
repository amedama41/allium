#ifndef CANARD_PACKET_PARSER_HPP
#define CANARD_PACKET_PARSER_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <iterator>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/mac_address.hpp>

namespace canard {

    namespace detail {
        template <class T>
        auto decode(std::uint8_t const* const data)
            -> T
        {
            auto value = T{};
            std::copy_n(data, sizeof(value), canard::as_byte_range(value).begin());
            return value;
        }
    } // namespace detail

    class ether_header
    {
    public:
        explicit ether_header(std::uint8_t const* data)
            : data_{data}
        {
        }

        auto destination() const
            -> canard::mac_address
        {
            auto dst = std::array<std::uint8_t, 6>{};
            std::copy_n(data_ + offsetof(::ether_header, ether_dhost), sizeof(dst), dst.data());
            return canard::mac_address{dst};
        }

        auto source() const
            -> canard::mac_address
        {
            auto src = std::array<std::uint8_t, 6>{};
            std::copy_n(data_ + offsetof(::ether_header, ether_shost), sizeof(src), src.data());
            return canard::mac_address{src};
        }

        auto ether_type() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ether_header, ether_type)));
        }

        auto next() const
            -> std::uint8_t const*
        {
            return data_ + sizeof(::ether_header);
        }

    private:
        std::uint8_t const* data_;
    };

    class vlan_tag
    {
    public:
        vlan_tag(std::uint8_t const* const data)
            : data_{data}
        {
        }

        auto vid() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_)) & 0x0FFF;
        }

        auto pcp() const
            -> std::uint8_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_)) >> 5;
        }

        auto ether_type() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t)));
        }

        auto next() const
            -> std::uint8_t const*
        {
            return data_ + sizeof(std::uint32_t);
        }

    private:
        std::uint8_t const* data_;
    };

    class arp
    {
    public:
        arp(std::uint8_t const* const data)
            : data_{data}
        {
        }

        auto length() const
            -> std::uint64_t
        {
            return sizeof(::arphdr) + hardware_length() * 2 + protocol_length() * 2;
        }

        auto hardware_type() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::arphdr, ar_hrd)));
        }

        auto protocol_type() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::arphdr, ar_pro)));
        }

        auto hardware_length() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + offsetof(::arphdr, ar_hln));
        }

        auto protocol_length() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + offsetof(::arphdr, ar_pln));
        }

        auto operation() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::arphdr, ar_op)));
        }

        auto sender_hardware_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return {data_ + sizeof(::arphdr), data_ + sizeof(::arphdr) + hardware_length()};
        }

        auto sender_protocol_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            auto const begin = data_ + sizeof(::arphdr) + hardware_length();
            return {begin, begin + protocol_length()};
        }

        auto target_hardware_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            auto const begin = data_ + sizeof(::arphdr) + hardware_length() + protocol_length();
            return {begin, begin + hardware_length()};
        }

        auto target_protocol_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            auto const begin = data_ + sizeof(::arphdr) + hardware_length() * 2 + protocol_length();
            return {begin, begin + protocol_length()};
        }

    private:
        std::uint8_t const* data_;
    };

    class lldpdu
    {
    public:
        lldpdu(std::uint8_t const* const first, std::uint8_t const* const last)
            : data_(first)
            , last_(last)
        {
        }

        auto chassis_id() const
            -> std::string
        {
            auto const tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(data_));
            return std::string{
                  data_ + sizeof(std::uint16_t) + sizeof(std::uint8_t)
                , data_ + sizeof(std::uint16_t) + tlv_length(tlv_header)
            };
        }

        auto chassis_id_subtype() const
            -> std::uint8_t
        {
            return *(data_ + sizeof(std::uint16_t));
        }

        auto port_id() const
            -> std::string
        {
            auto const chassis_id_tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(data_));
            auto const first = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
            auto const tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(first));
            return std::string{
                  first + sizeof(std::uint16_t) + sizeof(std::uint8_t)
                , first + sizeof(std::uint16_t) + tlv_length(tlv_header)
            };
        }

        auto port_id_subtype() const
            -> std::uint8_t
        {
            auto const chassis_id_tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(data_));
            auto const first = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
            return *(first + sizeof(std::uint16_t));
        }

        auto time_to_live() const
            -> std::uint16_t
        {
            auto const chassis_id_tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(data_));
            auto first = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
            auto const port_id_tlv_header
                = boost::endian::big_to_native(detail::decode<std::uint16_t>(first));
            first += sizeof(std::uint16_t) + tlv_length(port_id_tlv_header);
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(first + sizeof(std::uint16_t)));
        }

    private:
        static auto tlv_length(std::uint16_t const tlv_header)
            -> std::uint16_t
        {
            return tlv_header & 0x1FF;
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class ipv4_header
    {
    public:
        ipv4_header(std::uint8_t const* const data, std::uint8_t const* last)
            : data_{data}, last_{last}
        {
        }

        auto version() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_) >> 4;
        }

        auto length() const
            -> std::uint8_t
        {
            return (detail::decode<std::uint8_t>(data_) & 0x0F) << 2;
        }

        auto type_of_service() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + offsetof(::ip, ip_tos));
        }

        auto dscp() const
            -> std::uint8_t
        {
            return type_of_service() >> 2;
        }

        auto ecn() const
            -> std::uint8_t
        {
            return type_of_service() & 0x03;
        }

        auto total_length() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ip, ip_len)));
        }

        auto identification() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ip, ip_id)));
        }

        auto flags() const
            -> std::uint8_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ip, ip_off))) >> 13;
        }

        auto fragment_offset() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ip, ip_off))) & 0x1FFF;
        }

        auto time_to_live() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + offsetof(::ip, ip_ttl));
        }

        auto protocol() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + offsetof(::ip, ip_p));
        }

        auto checksum() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + offsetof(::ip, ip_sum)));
        }

        auto source() const
            -> boost::asio::ip::address_v4
        {
            auto addr = detail::decode<std::uint32_t>(data_ + offsetof(::ip, ip_src));
            return boost::asio::ip::address_v4{
                boost::endian::big_to_native(addr)
            };
        }

        auto destination() const
            -> boost::asio::ip::address_v4
        {
            auto addr = detail::decode<std::uint32_t>(data_ + offsetof(::ip, ip_dst));
            return boost::asio::ip::address_v4{
                boost::endian::big_to_native(addr)
            };
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(length(), std::distance(data_, last_)));
        }

        auto end() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(total_length(), std::distance(data_, last_)));
        }

        auto payload() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return boost::iterator_range<std::uint8_t const*>{next(), end()};
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class icmpv4
    {
    public:
        static std::uint64_t const min_length = 8;

        icmpv4(std::uint8_t const* data, std::uint8_t const* last)
            : data_{data}, last_{last}
        {
        }

        auto type() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_);
        }

        auto code() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint8_t));
        }

        auto checksum() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t)));
        }

        auto payload() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return {data_ + sizeof(std::uint64_t), last_};
        }

    protected:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class icmpv4_echo
        : public icmpv4
    {
    public:
        static std::uint64_t const min_length = 8;

        explicit icmpv4_echo(icmpv4 const& header)
            : icmpv4{header}
        {
        }

        auto identifier() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t));
        }

        auto sequence_number() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t) + sizeof(std::uint16_t));
        }
    };

    class icmpv4_redirect
        : public icmpv4
    {
    public:
        static std::uint64_t const min_length = 8;

        explicit icmpv4_redirect(icmpv4 const& header)
            : icmpv4{header}
        {
        }

        auto gateway_address() const
            -> boost::asio::ip::address_v4
        {
            return boost::asio::ip::address_v4{
                boost::endian::big_to_native(
                        detail::decode<std::uint32_t>(data_ + sizeof(std::uint32_t)))
            };
        }
    };

    class icmpv4_timestamp
        : public icmpv4
    {
    public:
        static std::uint64_t const min_length = 20;

        explicit icmpv4_timestamp(icmpv4 const& header)
            : icmpv4{header}
        {
        }

        auto identifier() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t));
        }

        auto sequence_number() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t) + sizeof(std::uint16_t));
        }

        auto originate_timestamp() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_ + sizeof(std::uint32_t)));
        }

        auto receive_timestamp() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_ + sizeof(std::uint64_t)));
        }

        auto transmit_timestamp() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_ + sizeof(std::uint64_t) + sizeof(std::uint32_t)));
        }
    };

    class icmpv4_address_mask
        : public icmpv4
    {
    public:
        static std::uint64_t const min_length = 12;

        explicit icmpv4_address_mask(icmpv4 const& header)
            : icmpv4{header}
        {
        }

        auto identifier() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t));
        }

        auto sequence_number() const
            -> std::uint16_t
        {
            return detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t) + sizeof(std::uint16_t));
        }

        auto address_mask() const
            -> boost::asio::ip::address_v4
        {
            return boost::asio::ip::address_v4{
                boost::endian::big_to_native(
                        detail::decode<std::uint32_t>(data_ + sizeof(std::uint32_t)))
            };
        };
    };

    class ipv6_header
    {
    public:
        ipv6_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto version() const
            -> std::uint8_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_))
                >> 28;
        }

        auto traffic_class() const
            -> std::uint8_t
        {
            return (boost::endian::big_to_native(
                        detail::decode<std::uint32_t>(data_))
                    >> 20) & 0x0FF;
        }

        auto dscp() const
            -> std::uint8_t
        {
            return traffic_class() >> 2;
        }

        auto ecn() const
            -> std::uint8_t
        {
            return traffic_class() >> 0x03;
        }

        auto flow_label() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_))
                & 0x0FFFFF;
        }

        auto payload_length() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint32_t)));
        }

        auto next_header() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint32_t) + sizeof(std::uint16_t));
        }

        auto hop_limit() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(std::uint8_t));
        }

        auto source() const
            -> boost::asio::ip::address_v6
        {
            auto addr = boost::asio::ip::address_v6::bytes_type{};
            std::copy_n(data_ + sizeof(std::uint64_t), addr.size(), addr.data());
            return boost::asio::ip::address_v6{addr};
        }

        auto destination() const
            -> boost::asio::ip::address_v6
        {
            auto addr = boost::asio::ip::address_v6::bytes_type{};
            std::copy_n(data_ + sizeof(std::uint64_t) * 3, addr.size(), addr.data());
            return boost::asio::ip::address_v6{addr};
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, 40);
        }

        auto end() const
            -> std::uint8_t const*
        {
            auto const length = payload_length();
            return length == 0 ? last_
                : std::next(next(), std::min<std::size_t>(length, std::distance(next(), last_)));
        }

        auto payload() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return {next(), end()};
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class ipv6_extension_header
    {
    public:
        ipv6_extension_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto next_header() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_);
        }

        auto extension_header_length() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint8_t));
        }

        auto length() const
            -> std::uint64_t
        {
            return (extension_header_length() + 1) * sizeof(std::uint64_t);
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(length(), std::distance(data_, last_)));
        }

        auto end() const
            -> std::uint8_t const*
        {
            return last_;
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class ipv6_fragment_header
    {
    public:
        ipv6_fragment_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto next_header() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_);
        }

        auto length() const
            -> std::uint64_t
        {
            return sizeof(std::uint64_t);
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(length(), std::distance(data_, last_)));
        }

        auto end() const
            -> std::uint8_t const*
        {
            return last_;
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class ah_header
    {
    public:
        ah_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto next_header() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_);
        }

        auto next_length() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint8_t));
        }

        auto length() const
            -> std::uint64_t
        {
            return (next_length() + 2) * sizeof(std::uint32_t);
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(length(), std::distance(data_, last_)));
        }

        auto end() const
            -> std::uint8_t const*
        {
            return last_;
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class icmpv6
    {
    public:
        icmpv6(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto type() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_);
        }

        auto code() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint8_t));
        }

        auto checksum() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint8_t>(data_ + sizeof(std::uint16_t)));
        }

    protected:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class icmpv6_neighbor_solicitation
        : public icmpv6
    {
    public:
        static std::uint64_t const min_length = 24;

        icmpv6_neighbor_solicitation(icmpv6 const& header)
            : icmpv6{header}
        {
        }

        auto target_address() const
            -> boost::asio::ip::address_v6
        {
            auto addr = boost::asio::ip::address_v6::bytes_type{};
            std::copy_n(data_ + sizeof(std::uint64_t), addr.size(), addr.data());
            return boost::asio::ip::address_v6{addr};
        }

        auto source_link_layer_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return boost::iterator_range<std::uint8_t const*>{data_ + sizeof(std::uint64_t) * 3, last_};
        }
    };

    class icmpv6_neighbor_advertisement
        : public icmpv6
    {
    public:
        static std::uint64_t const min_length = 24;

        icmpv6_neighbor_advertisement(icmpv6 const& header)
            : icmpv6{header}
        {
        }

        auto target_address() const
            -> boost::asio::ip::address_v6
        {
            auto addr = boost::asio::ip::address_v6::bytes_type{};
            std::copy_n(data_ + sizeof(std::uint64_t), addr.size(), addr.data());
            return boost::asio::ip::address_v6{addr};
        }

        auto target_link_layer_address() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return boost::iterator_range<std::uint8_t const*>{data_ + sizeof(std::uint64_t) * 3, last_};
        }
    };

    class tcp_header
    {
    public:
        tcp_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto source() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_));
        }

        auto destination() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t)));
        }

        auto sequence_number() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_ + sizeof(std::uint32_t)));
        }

        auto acknowledgement_number() const
            -> std::uint32_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint32_t>(data_ + sizeof(std::uint64_t)));
        }

        auto length() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint64_t) + sizeof(std::uint32_t)) >> 2;
        }

        auto flags() const
            -> std::uint8_t
        {
            return detail::decode<std::uint8_t>(data_ + sizeof(std::uint64_t) + sizeof(std::uint32_t) + sizeof(std::uint8_t));
        }

        auto window() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint64_t) + sizeof(std::uint32_t) + sizeof(std::uint16_t)));
        }

        auto checksum() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint64_t) * 2));
        }

        auto urgent_pointer() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint64_t) * 2 + sizeof(std::uint16_t)));
        }

        auto next() const
            -> std::uint8_t const*
        {
            return std::next(data_, std::min<std::size_t>(length(), std::distance(data_, last_)));
        }

        auto end() const
            -> std::uint8_t const*
        {
            return last_;
        }

        auto payload() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return {next(), end()};
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    class udp_header
    {
    public:
        udp_header(std::uint8_t const* const data, std::uint8_t const* const last)
            : data_{data}, last_{last}
        {
        }

        auto source() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_));
        }

        auto destination() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t)));
        }

        auto payload_length() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t) * 2));
        }

        auto checksum() const
            -> std::uint16_t
        {
            return boost::endian::big_to_native(
                    detail::decode<std::uint16_t>(data_ + sizeof(std::uint16_t) * 3));
        }

        auto next() const
            -> std::uint8_t const*
        {
            return data_ + sizeof(std::uint16_t) * 4;
        }

        auto end() const
            -> std::uint8_t const*
        {
            return last_;
        }

        auto payload() const
            -> boost::iterator_range<std::uint8_t const*>
        {
            return boost::make_iterator_range(next(), end());
        }

    private:
        std::uint8_t const* data_;
        std::uint8_t const* last_;
    };

    template <class Function>
    inline void parse_tcp(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < 20) {
            return;
        }
        auto const header = tcp_header{first, last};
        if (!f(header) || std::distance(first, last) < header.length()) {
            return;
        }
        f(header.payload());
    }

    template <class Function>
    inline void parse_udp(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < 8) {
            return;
        }
        auto const header = udp_header{first, last};
        if (f(header)) {
            f(header.payload());
        }
    }

    template <class Function>
    inline void parse_icmpv6(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < 8) {
            return;
        }
        auto const header = icmpv6{first, last};
        switch (header.type()) {
        case 135:
            if (std::distance(first, last) >= icmpv6_neighbor_solicitation::min_length) {
                f(icmpv6_neighbor_solicitation{header});
                return;
            }
            break;
        case 136:
            if (std::distance(first, last) >= icmpv6_neighbor_advertisement::min_length) {
                f(icmpv6_neighbor_advertisement{header});
                return;
            }
            break;
        }
        f(header);
    }

    template <class Function>
    inline void parse_icmpv4(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < 8) {
            return;
        }
        auto const header = icmpv4{first, last};
        switch (header.type()) {
        case 0: case 8:
            if (std::distance(first, last) >= icmpv4_echo::min_length) {
                f(icmpv4_echo(header));
                return;
            }
            break;
        case 5:
            if (std::distance(first, last) >= icmpv4_redirect::min_length) {
                f(icmpv4_redirect{header});
                return;
            }
            break;
        case 9: case 10:
            // TODO
            break;
        case 13: case 14:
            if (std::distance(first, last) >= icmpv4_timestamp::min_length) {
                f(icmpv4_timestamp{header});
                return;
            }
            break;
        case 17: case 18:
            if (std::distance(first, last) >= icmpv4_address_mask::min_length) {
                f(icmpv4_address_mask{header});
                return;
            }
            break;
        }
        f(header);
    }

    template <class Function>
    inline void ip_next_header(std::uint8_t const protocol, std::uint8_t const* const first, std::uint8_t const* const last, Function& f);

    template <class Function>
    inline void parse_ipv6_hop_by_hop(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(std::uint64_t)) {
            return;
        }
        auto const header = ipv6_extension_header{first, last};
        if (!f(header) || std::distance(first, last) < header.length()) {
            return;
        }
        ip_next_header(header.next_header(), header.next(), header.end(), f);
    }

    template <class Function>
    inline void parse_ipv6_fragment(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(std::uint64_t)) {
            return;
        }
        auto const header = ipv6_fragment_header{first, last};
        if (f(header)) {
            ip_next_header(header.next_header(), header.next(), header.end(), f);
        }
    }

    template <class Function>
    inline void parse_ah_header(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(std::uint64_t)) {
            return;
        }
        auto const header = ah_header{first, last};
        if (!f(header) || std::distance(first, last) < header.length()) {
            return;
        }
        ip_next_header(header.next_header(), header.next(), header.end(), f);
    }

    template <class Function>
    inline void ip_next_header(std::uint8_t const protocol, std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        switch (protocol) {
        case 0:
            parse_ipv6_hop_by_hop(first, last, f);
            return;
        case 1:
            parse_icmpv4(first, last, f);
            return;
        case 6:
            parse_tcp(first, last, f);
            return;
        case 17:
            parse_udp(first, last, f);
            return;
        // case 41: return;
        case 43:
            // parse_ipv6_router(first, last, f);
            parse_ipv6_hop_by_hop(first, last, f);
            return;
        case 46:
            parse_ipv6_fragment(first, last, f);
            return;
        case 51:
            parse_ah_header(first, last, f);
            return;
        case 58:
            parse_icmpv6(first, last, f);
            return;
        case 59: // no-next
            // do nothing
            return;
        case 60:
            // parse_ipv6_options(first, last, f);
            parse_ipv6_hop_by_hop(first, last, f);
            return;
        case 132:
            // parse_sctp(first, last, f);
            return;
        }
        f(boost::make_iterator_range(first, last));
    }

    template <class Function>
    inline void parse_ipv6(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < 40) {
            return;
        }
        auto const header = ipv6_header{first, last};
        if (f(header)) {
            ip_next_header(header.next_header(), header.next(), header.end(), f);
        }
    }

    template <class Function>
    inline void parse_ipv4(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(::ip)) {
            return;
        }
        auto const header = ipv4_header{first, last};
        if (!f(header) || std::distance(first, last) <= header.length()) {
            return;
        }

        if (header.fragment_offset() == 0) {
            ip_next_header(header.protocol(), header.next(), header.end(), f);
        }
        else {
            f(header.payload());
        }
    }

    template <class Function>
    inline void parse_arp(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(::arphdr)) {
            return;
        }
        auto const header = arp{first};
        if (std::distance(first, last) < header.length()) {
            return;
        }
        f(header);
    }

    template <class Function>
    inline void parse_lldpdu(std::uint8_t const* const first, std::uint8_t const* const last, Function&& f)
    {
        auto const header = lldpdu{first, last};
        f(header);
    }

    template <class Function>
    inline void ether_next_header(std::uint16_t const type, std::uint8_t const* const first, std::uint8_t const* const last, Function& f);

    template <class Function>
    inline void parse_vlan_tag(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        if (std::distance(first, last) < sizeof(std::uint16_t) * 2) {
            return;
        }

        auto const header = vlan_tag{first};
        if (f(header)) {
            ether_next_header(header.ether_type(), header.next(), last, f);
        }
    }

    template <class Function>
    inline void ether_next_header(std::uint16_t const type, std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        // TODO if ( type <= ETHERMTU )
        switch (type) {
        case ETHERTYPE_IP:
            parse_ipv4(first, last, f);
            return;
        case ETHERTYPE_ARP:
            parse_arp(first, last, f);
            return;
        case ETHERTYPE_VLAN:
            parse_vlan_tag(first, last, f);
            return;
        case ETHERTYPE_IPV6:
            parse_ipv6(first, last, f);
            return;
        case 0x88CC:
            parse_lldpdu(first, last, f);
            return;
        default:
            f(boost::make_iterator_range(first, last));
            return;
        }
    }

    template <class Function>
    inline void parse_ether_frame(std::uint8_t const* const first, std::uint8_t const* const last, Function& f)
    {
        static_assert(sizeof(::ether_header) == 14, "ether_header size must be 14 bytes");
        if (std::distance(first, last) < sizeof(::ether_header)) {
            return;
        }

        auto const header = ether_header{first};
        if (f(header)) {
            ether_next_header(header.ether_type(), header.next(), last, f);
        }
    }

    template <class Range, class Function>
    inline void for_each_header(Range const& frame, Function f)
    {
        parse_ether_frame(frame.data(), frame.data() + frame.size(), f);
    }

    template <class Function, class T>
    struct hoge
    {
        template <class Header>
        auto operator()(Header&&) const
            -> bool
        {
            return true;
        }

        auto operator()(T header) const
            -> bool
        {
            function(header);
            return false;
        }

        Function function;
    };

    class packet
    {
    public:
        packet(std::uint8_t const* const first, std::uint8_t const* const last)
            : first_{first}, last_{last}
        {
        }

        template <class Range>
        explicit packet(Range const& frame) noexcept
            : first_(frame.data()), last_(frame.data() + frame.size())
        {
        }

        template <class Function>
        auto ether_header(Function&& function) const
            -> packet const&
        {
            auto func = hoge<Function, canard::ether_header>{std::forward<Function>(function)};
            parse_ether_frame(first_, last_, func);
            return *this;
        }

        template <class Function>
        auto lldpdu(Function&& function) const
            -> packet const&
        {
            auto func = hoge<Function, canard::lldpdu>{std::forward<Function>(function)};
            parse_ether_frame(first_, last_, func);
            return *this;
        }

        template <class Function>
        void for_each(Function&& function) const
        {
        }

    private:
        std::uint8_t const* first_;
        std::uint8_t const* last_;
    };

} // namespace canard

#endif // CANARD_PACKET_PARSER_HPP
