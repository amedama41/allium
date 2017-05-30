#ifndef CANARD_PACKET_PARSER_HPP
#define CANARD_PACKET_PARSER_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <iterator>
#include <string>
#include <utility>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/range/iterator_range.hpp>
#include <canard/mac_address.hpp>

namespace canard {

  namespace detail {

    template <class T>
    auto decode(std::uint8_t const* const data) noexcept
      -> T
    {
      auto value = T{};
      std::memcpy(&value, data, sizeof(value));
      return boost::endian::big_to_native(value);
    }

    template <std::size_t N>
    auto decode_array(std::uint8_t const* const data) noexcept
      -> std::array<std::uint8_t, N>
    {
      auto array = std::array<std::uint8_t, N>{};
      std::memcpy(array.data(), data, array.size());
      return array;
    }

  } // namespace detail

  template <class Function>
  inline void ether_next_header(
        std::uint16_t const type
      , std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f);

  class ether_header
  {
  public:
    static constexpr std::uint16_t min_header_length = 14;
    static constexpr std::uint16_t min_ether_type = 0x0600;
    static constexpr std::uint16_t min_frame_length = 1500;
    enum offset
    {
      DESTINATION = 0,
      SOURCE = DESTINATION + 6,
      ETHER_TYPE = SOURCE + 6,
      END = ETHER_TYPE + 2,
    };

    explicit ether_header(std::uint8_t const* data) noexcept
      : data_{data}
    {
    }

    auto destination() const
      -> canard::mac_address
    {
      return canard::mac_address{
        detail::decode_array<6>(data_ + offset::DESTINATION)
      };
    }

    auto source() const
      -> canard::mac_address
    {
      return canard::mac_address{
        detail::decode_array<6>(data_ + offset::SOURCE)
      };
    }

    auto ether_type() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::ETHER_TYPE);
    }

    auto next() const noexcept
      -> std::uint8_t const*
    {
      return data_ + min_header_length;
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < min_header_length) {
        return;
      }

      auto const header = ether_header{first};
      auto const type_of_length = header.ether_type();
      if (type_of_length >= min_ether_type) {
        if (f(header)) {
          ether_next_header(type_of_length, header.next(), last, f);
        }
      }
      else if (type_of_length <= min_frame_length) {
        // TODO
      }
      else {
        // TODO
      }
    }

  private:
    std::uint8_t const* data_;
  };

  class vlan_tag
  {
  public:
    static constexpr std::uint16_t type = 0x8100;
    static constexpr std::uint16_t vlan_tag_length = 4;
    enum offset
    {
      TCI = 0,
      ETHER_TYPE = TCI + 2,
      END = ETHER_TYPE + 2,
    };

    explicit vlan_tag(std::uint8_t const* const data) noexcept
      : data_{data}
    {
    }

    auto vid() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::TCI) & 0x0fff;
    }

    auto pcp() const
      -> std::uint8_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::TCI) >> 5;
    }

    auto ether_type() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::ETHER_TYPE);
    }

    auto next() const noexcept
      -> std::uint8_t const*
    {
      return data_ + vlan_tag_length;
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < vlan_tag_length) {
        return;
      }

      auto const header = vlan_tag{first};
      if (f(header)) {
        ether_next_header(header.ether_type(), header.next(), last, f);
      }
    }

  private:
    std::uint8_t const* data_;
  };

  class arp
  {
  public:
    static constexpr std::uint16_t type = 0x0806;
    static constexpr std::uint16_t min_length = 8;
    enum offset
    {
      HARDWARE_TYPE = 0,
      PROTOCOL_TYPE = HARDWARE_TYPE + 2,
      HARDWARE_LENGTH = PROTOCOL_TYPE + 2,
      PROTOCOL_LENGTH = HARDWARE_LENGTH + 1,
      OPERATION = PROTOCOL_TYPE + 1,
      SENDER_HARDWARE_ADDRESS = OPERATION + 2,
    };

    explicit arp(std::uint8_t const* const data) noexcept
      : data_{data}
    {
    }

    auto length() const
      -> std::uint16_t
    {
      return min_length + hardware_length() * 2 + protocol_length() * 2;
    }

    auto hardware_type() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::HARDWARE_TYPE);
    }

    auto protocol_type() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::PROTOCOL_TYPE);
    }

    auto hardware_length() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::HARDWARE_LENGTH);
    }

    auto protocol_length() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::PROTOCOL_LENGTH);
    }

    auto operation() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::OPERATION);
    }

    auto sender_hardware_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      auto const begin = data_ + min_length;
      return {begin, begin + hardware_length()};
    }

    auto sender_protocol_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      auto const begin = data_ + min_length + hardware_length();
      return {begin, begin + protocol_length()};
    }

    auto target_hardware_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      auto const begin
        = data_ + min_length + hardware_length() + protocol_length();
      return {begin, begin + hardware_length()};
    }

    auto target_protocol_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      auto const begin
        = data_ + min_length + hardware_length() * 2 + protocol_length();
      return {begin, begin + protocol_length()};
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < min_length) {
        return;
      }
      auto const header = arp{first};
      if (std::distance(first, last) < header.length()) {
        return;
      }
      f(header);
    }

  private:
    std::uint8_t const* data_;
  };

  class lldpdu
  {
  public:
    static constexpr std::uint16_t type = 0x88CC;

    lldpdu(std::uint8_t const* const first, std::uint8_t const* const last)
      : data_(first)
      , last_(last)
    {
    }

    auto chassis_id() const
      -> std::string
    {
      auto const tlv_header = detail::decode<std::uint16_t>(data_);
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
      auto const chassis_id_tlv_header = detail::decode<std::uint16_t>(data_);
      auto const first
        = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
      auto const tlv_header = detail::decode<std::uint16_t>(first);
      return std::string{
          first + sizeof(std::uint16_t) + sizeof(std::uint8_t)
        , first + sizeof(std::uint16_t) + tlv_length(tlv_header)
      };
    }

    auto port_id_subtype() const
      -> std::uint8_t
    {
      auto const chassis_id_tlv_header = detail::decode<std::uint16_t>(data_);
      auto const first
        = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
      return *(first + sizeof(std::uint16_t));
    }

    auto time_to_live() const
      -> std::uint16_t
    {
      auto const chassis_id_tlv_header = detail::decode<std::uint16_t>(data_);
      auto first
        = data_ + sizeof(std::uint16_t) + tlv_length(chassis_id_tlv_header);
      auto const port_id_tlv_header = detail::decode<std::uint16_t>(first);
      first += sizeof(std::uint16_t) + tlv_length(port_id_tlv_header);
      return detail::decode<std::uint16_t>(first + sizeof(std::uint16_t));
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function&& f)
    {
      auto const header = lldpdu{first, last};
      f(header);
    }

  private:
    static auto tlv_length(std::uint16_t const tlv_header)
      -> std::uint16_t
    {
      return tlv_header & 0x1ff;
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  template <class Function>
  inline void ip_next_header(
        std::uint8_t const protocol, std::uint8_t const* const first
      , std::uint8_t const* const last, Function& f);

  class ipv4_header
  {
  public:
    static constexpr std::uint16_t type = 0x0800;
    static constexpr std::uint16_t min_header_length = 20;
    enum offset
    {
      VERSION_AND_LENGTH = 0,
      TYPE_OF_SERVICE = VERSION_AND_LENGTH + 1,
      TOTAL_LENGTH = TYPE_OF_SERVICE + 1,
      IDENTIFICATION = TOTAL_LENGTH + 2,
      FLAGS = IDENTIFICATION + 2,
      FLAGS_AND_FRAGMENT_OFFSET = FLAGS,
      TIME_TO_LIVE = FLAGS_AND_FRAGMENT_OFFSET + 2,
      PROTOCOL = TIME_TO_LIVE + 1,
      CHECKSUM = PROTOCOL + 1,
      SOURCE_ADDRESS = CHECKSUM + 2,
      DESTINATION_ADDRESS = SOURCE_ADDRESS + 4,
      OPTIONS_OR_DATA = DESTINATION_ADDRESS + 4,
    };

    ipv4_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto version() const
      -> std::uint8_t
    {
      auto const version_and_length
        = detail::decode<std::uint8_t>(data_ + offset::VERSION_AND_LENGTH);
      return version_and_length >> 4;
    }

    auto length() const
      -> std::uint8_t
    {
      auto const version_and_length
        = detail::decode<std::uint8_t>(data_ + offset::VERSION_AND_LENGTH);
      return (version_and_length & 0x0f) * sizeof(std::uint32_t);
    }

    auto type_of_service() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::TYPE_OF_SERVICE);
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
      return detail::decode<std::uint16_t>(data_ + offset::TOTAL_LENGTH);
    }

    auto identification() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::IDENTIFICATION);
    }

    auto flags() const
      -> std::uint8_t
    {
      auto const flags = detail::decode<std::uint8_t>(data_ + offset::FLAGS);
      return flags >> 5;
    }

    auto fragment_offset() const
      -> std::uint16_t
    {
      auto const flags_and_fragment_offset = detail::decode<std::uint16_t>(
          data_ + offset::FLAGS_AND_FRAGMENT_OFFSET);
      return flags_and_fragment_offset & 0x1fff;
    }

    auto time_to_live() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::TIME_TO_LIVE);
    }

    auto protocol() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::PROTOCOL);
    }

    auto checksum() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::CHECKSUM);
    }

    auto source_address() const
      -> boost::asio::ip::address_v4
    {
      return boost::asio::ip::address_v4{
        detail::decode<std::uint32_t>(data_ + offset::SOURCE_ADDRESS)
      };
    }

    auto destination_address() const
      -> boost::asio::ip::address_v4
    {
      return boost::asio::ip::address_v4{
        detail::decode<std::uint32_t>(data_ + offset::DESTINATION_ADDRESS)
      };
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, length());
    }

    auto end() const
      -> std::uint8_t const*
    {
      auto const total_length = std::size_t{this->total_length()};
      return data_length() < total_length
        ? last_
        : std::next(data_, total_length);
    }

    auto payload() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return boost::iterator_range<std::uint8_t const*>{next(), end()};
    }

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < min_header_length) {
        return;
      }
      auto const header = ipv4_header{first, last};
      if (!f(header) || std::distance(first, last) < header.length()) {
        return;
      }

      if (header.fragment_offset() == 0) {
        ip_next_header(header.protocol(), header.next(), header.end(), f);
      }
      else {
        f(header.payload());
      }
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class icmpv4
  {
  public:
    static constexpr std::uint8_t protocol = 1;
    static constexpr std::uint16_t header_length = 4;
    enum offset
    {
      TYPE = 0,
      CODE = TYPE + 1,
      CHECKSUM = CODE + 1,
      DATA = CHECKSUM + 2,
    };

    icmpv4(std::uint8_t const* data, std::uint8_t const* last)
      : data_{data}, last_{last}
    {
    }

    auto type() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::TYPE);
    }

    auto code() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::CODE);
    }

    auto checksum() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::CHECKSUM);
    }

    auto payload() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return {data_ + header_length, last_};
    }

  protected:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class icmpv4_echo
    : public icmpv4
  {
  public:
    static constexpr std::uint8_t request_type = 0;
    static constexpr std::uint8_t reply_type = 8;
    static constexpr std::uint16_t min_length = 8;
    enum offset
    {
      IDENTIFIER = icmpv4::header_length + 0,
      SEQUENCE_NUMBER = IDENTIFIER + 2,
      DATA = SEQUENCE_NUMBER + 2,
    };

    explicit icmpv4_echo(icmpv4 const& header) noexcept
      : icmpv4{header}
    {
    }

    auto identifier() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::IDENTIFIER);
    }

    auto sequence_number() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::SEQUENCE_NUMBER);
    }
  };

  class icmpv4_redirect
    : public icmpv4
  {
  public:
    static constexpr std::uint8_t icmp_type = 5;
    static constexpr std::uint16_t min_length = 8;
    enum offset
    {
      GATEWAY_ADDRESS = icmpv4::header_length + 0,
      ORIGINAL_MESSAGE = GATEWAY_ADDRESS + 4,
    };

    explicit icmpv4_redirect(icmpv4 const& header) noexcept
      : icmpv4{header}
    {
    }

    auto gateway_address() const
      -> boost::asio::ip::address_v4
    {
      return boost::asio::ip::address_v4{
        detail::decode<std::uint32_t>(data_ + offset::GATEWAY_ADDRESS)
      };
    }
  };

  class icmpv4_timestamp
    : public icmpv4
  {
  public:
    static constexpr std::uint8_t request_type = 13;
    static constexpr std::uint8_t reply_type = 14;
    static constexpr std::uint16_t min_length = 20;
    enum offset
    {
      IDENTIFIER = icmpv4::header_length + 0,
      SEQUENCE_NUMBER = IDENTIFIER + 2,
      ORIGINATE_TIMESTAMP = SEQUENCE_NUMBER + 2,
      RECEIVE_TIMESTAMP = ORIGINATE_TIMESTAMP + 4,
      TRANSMIT_TIMESTAMP = RECEIVE_TIMESTAMP + 4,
      END = TRANSMIT_TIMESTAMP + 4,
    };

    explicit icmpv4_timestamp(icmpv4 const& header) noexcept
      : icmpv4{header}
    {
    }

    auto identifier() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::IDENTIFIER);
    }

    auto sequence_number() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::SEQUENCE_NUMBER);
    }

    auto originate_timestamp() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::ORIGINATE_TIMESTAMP);
    }

    auto receive_timestamp() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::RECEIVE_TIMESTAMP);
    }

    auto transmit_timestamp() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::TRANSMIT_TIMESTAMP);
    }
  };

  class icmpv4_address_mask
    : public icmpv4
  {
  public:
    static constexpr std::uint8_t request_type = 17;
    static constexpr std::uint8_t reply_type = 18;
    static constexpr std::uint16_t min_length = 12;
    enum offset
    {
      IDENTIFIER = icmpv4::header_length + 0,
      SEQUENCE_NUMBER = IDENTIFIER + 2,
      ADDRESS_MASK = SEQUENCE_NUMBER + 2,
      END = ADDRESS_MASK + 4,
    };

    explicit icmpv4_address_mask(icmpv4 const& header) noexcept
      : icmpv4{header}
    {
    }

    auto identifier() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::IDENTIFIER);
    }

    auto sequence_number() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::SEQUENCE_NUMBER);
    }

    auto address_mask() const
      -> boost::asio::ip::address_v4
    {
      return boost::asio::ip::address_v4{
        detail::decode<std::uint32_t>(data_ + offset::ADDRESS_MASK)
      };
    };
  };

  class ipv6_header
  {
  public:
    static constexpr std::uint16_t type = 0x86dd;
    static constexpr std::uint16_t header_length = 40;
    enum offset
    {
      VERSION = 0,
      VERSION_AND_TRAFFIC_CLASS_AND_FLOW_LABEL = VERSION,
      PAYLOAD_LENGTH = VERSION_AND_TRAFFIC_CLASS_AND_FLOW_LABEL + 4,
      NEXT_HEADER = PAYLOAD_LENGTH + 2,
      HOP_LIMIT = NEXT_HEADER + 1,
      SOURCE_ADDRESS = HOP_LIMIT + 1,
      DESTINATION_ADDRESS = SOURCE_ADDRESS + 16,
      DATA = DESTINATION_ADDRESS + 16,
    };

    ipv6_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto version() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::VERSION) >> 4;
    }

    auto traffic_class() const
      -> std::uint8_t
    {
      auto const version_and_traffic_class_and_flow_label
        = detail::decode<std::uint32_t>(
            data_ + offset::VERSION_AND_TRAFFIC_CLASS_AND_FLOW_LABEL);
      return (version_and_traffic_class_and_flow_label >> 20) & 0x0ff;
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
      auto const version_and_traffic_class_and_flow_label
        = detail::decode<std::uint32_t>(
            data_ + offset::VERSION_AND_TRAFFIC_CLASS_AND_FLOW_LABEL);
      return version_and_traffic_class_and_flow_label & 0x0fffff;
    }

    auto payload_length() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::PAYLOAD_LENGTH);
    }

    auto next_header() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::NEXT_HEADER);
    }

    auto hop_limit() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::HOP_LIMIT);
    }

    auto source_address() const
      -> boost::asio::ip::address_v6
    {
      return boost::asio::ip::address_v6{
        detail::decode_array<16>(data_ + offset::SOURCE_ADDRESS)
      };
    }

    auto destination_address() const
      -> boost::asio::ip::address_v6
    {
      return boost::asio::ip::address_v6{
        detail::decode_array<16>(data_ + offset::DESTINATION_ADDRESS)
      };
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, header_length);
    }

    auto end() const
      -> std::uint8_t const*
    {
      auto const payload_length = this->payload_length();
      return payload_length == 0 || data_length() < payload_length
        ? last_
        : std::next(data_, header_length + payload_length);
    }

    auto payload() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return {next(), end()};
    }

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& func)
    {
      if (std::distance(first, last) < header_length) {
        return;
      }
      auto const header = ipv6_header{first, last};
      if (func(header)) {
        ip_next_header(header.next_header(), header.next(), header.end(), func);
      }
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class ipv6_extension_header
  {
  public:
    static constexpr std::uint16_t min_header_length = 8;
    enum offset
    {
      NEXT_HEADER = 0,
      EXTENSION_HEADER_LENGTH = NEXT_HEADER + 1,
    };

    ipv6_extension_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto next_header() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::NEXT_HEADER);
    }

    auto extension_header_length() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(
          data_ + offset::EXTENSION_HEADER_LENGTH);
    }

    auto length() const
      -> std::uint16_t
    {
      return (extension_header_length() + 1) * sizeof(std::uint64_t);
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, length());
    }

    auto end() const noexcept
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
    static constexpr std::uint8_t protocol = 44;
    static constexpr std::uint16_t header_length = 8;
    enum offset
    {
      NEXT_HEADER = 0,
      RESERVED = NEXT_HEADER + 1,
      FRAGMENT_OFFSET_AND_FLAGS = RESERVED + 1,
      IDENTIFICATION = FRAGMENT_OFFSET_AND_FLAGS + 2,
      END = IDENTIFICATION + 4,
    };

    ipv6_fragment_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto next_header() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::NEXT_HEADER);
    }

    auto fragment_offset() const
      -> std::uint16_t
    {
      auto const fragment_offset_and_flags = detail::decode<std::uint16_t>(
          data_ + offset::FRAGMENT_OFFSET_AND_FLAGS);
      return fragment_offset_and_flags >> 3;
    }

    auto flags() const
      -> std::uint8_t
    {
      auto const fragment_offset_and_flags = detail::decode<std::uint16_t>(
            data_ + offset::FRAGMENT_OFFSET_AND_FLAGS);
      return fragment_offset_and_flags & 0x01;
    }

    auto identification() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::IDENTIFICATION);
    }

    auto length() const noexcept
      -> std::uint16_t
    {
      return header_length;
    }

    auto next() const noexcept
      -> std::uint8_t const*
    {
      return std::next(data_, header_length);
    }

    auto end() const
      -> std::uint8_t const*
    {
      return last_;
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < header_length) {
        return;
      }
      auto const header = ipv6_fragment_header{first, last};
      if (f(header)) {
        ip_next_header(header.next_header(), header.next(), header.end(), f);
      }
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class ah_header
  {
  public:
    static constexpr std::uint8_t protocol = 51;
    static constexpr std::uint16_t min_header_length = 12;
    enum offset
    {
      NEXT_HEADER = 0,
      PAYLOAD_LENGTH = NEXT_HEADER + 1,
      RESERVED = PAYLOAD_LENGTH + 1,
      SPI = RESERVED + 2,
      SEQUENCE_NUMBER = SPI + 4,
      AUTHENTICATION_DATA = SEQUENCE_NUMBER + 4,
    };

    ah_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto next_header() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::NEXT_HEADER);
    }

    auto payload_length() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::PAYLOAD_LENGTH);
    }

    auto length() const
      -> std::uint16_t
    {
      return (payload_length() + 2) * sizeof(std::uint32_t);
    }

    auto spi() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::SPI);
    }

    auto sequence_number() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::SEQUENCE_NUMBER);
    }

    auto authentication_data() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return {data_ + offset::AUTHENTICATION_DATA, data_ + length()};
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, length());
    }

    auto end() const noexcept
      -> std::uint8_t const*
    {
      return last_;
    }

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& f)
    {
      if (std::distance(first, last) < min_header_length) {
        return;
      }
      auto const header = ah_header{first, last};
      if (!f(header) || std::distance(first, last) < header.length()) {
        return;
      }
      ip_next_header(header.next_header(), header.next(), header.end(), f);
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class icmpv6
  {
  public:
    static constexpr std::uint8_t protocol = 58;
    static constexpr std::uint16_t header_length = 4;
    enum offset
    {
      TYPE = 0,
      CODE = TYPE + 1,
      CHECKSUM = CODE + 1,
      END = CHECKSUM + 2,
    };

    icmpv6(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto type() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::TYPE);
    }

    auto code() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::CODE);
    }

    auto checksum() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::CHECKSUM);
    }

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

  protected:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class icmpv6_neighbor_solicitation
    : public icmpv6
  {
  public:
    static constexpr std::uint8_t icmp_type = 135;
    static constexpr std::uint16_t min_length = 24;
    enum offset
    {
      TARGET_ADDRESS = icmpv6::header_length + 4,
      SOURCE_LINK_LAYER_ADDRESS = TARGET_ADDRESS + 16,
    };

    explicit icmpv6_neighbor_solicitation(icmpv6 const& header) noexcept
      : icmpv6{header}
    {
    }

    auto target_address() const
      -> boost::asio::ip::address_v6
    {
      return boost::asio::ip::address_v6{
        detail::decode_array<16>(data_ + offset::TARGET_ADDRESS)
      };
    }

    auto source_link_layer_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return boost::iterator_range<std::uint8_t const*>{
        data_ + offset::SOURCE_LINK_LAYER_ADDRESS, last_
      };
    }
  };

  class icmpv6_neighbor_advertisement
    : public icmpv6
  {
  public:
    static constexpr std::uint8_t icmp_type = 136;
    static constexpr std::uint16_t min_length = 24;
    enum offset
    {
      TARGET_ADDRESS = icmpv6::header_length + 4,
      TARGET_LINK_LAYER_ADDRESS = TARGET_ADDRESS + 16,
    };

    explicit icmpv6_neighbor_advertisement(icmpv6 const& header) noexcept
      : icmpv6{header}
    {
    }

    auto target_address() const
      -> boost::asio::ip::address_v6
    {
      return boost::asio::ip::address_v6{
        detail::decode_array<16>(data_ + offset::TARGET_ADDRESS)
      };
    }

    auto target_link_layer_address() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return boost::iterator_range<std::uint8_t const*>{
        data_ + offset::TARGET_LINK_LAYER_ADDRESS, last_
      };
    }
  };

  class tcp_header
  {
  public:
    static constexpr std::uint8_t protocol = 6;
    static constexpr std::uint16_t min_header_length = 20;
    enum offset
    {
      SOURCE_PORT = 0,
      DESTINATION_PORT = SOURCE_PORT + 2,
      SEQUENCE_NUMBER = DESTINATION_PORT + 2,
      ACKNOWLEDGEMENT_NUMBER = SEQUENCE_NUMBER + 4,
      DATA_OFFSET = ACKNOWLEDGEMENT_NUMBER + 4,
      DATA_OFFSET_AND_FLAGS = ACKNOWLEDGEMENT_NUMBER + 4,
      WINDOW = DATA_OFFSET_AND_FLAGS + 2,
      CHECKSUM = WINDOW + 2,
      URGENT_POINTER = CHECKSUM + 2,
      END = URGENT_POINTER + 2,
    };

    tcp_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto source_port() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::SOURCE_PORT);
    }

    auto destination_port() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::DESTINATION_PORT);
    }

    auto sequence_number() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(data_ + offset::SEQUENCE_NUMBER);
    }

    auto acknowledgement_number() const
      -> std::uint32_t
    {
      return detail::decode<std::uint32_t>(
          data_ + offset::ACKNOWLEDGEMENT_NUMBER);
    }

    auto data_offset() const
      -> std::uint8_t
    {
      return detail::decode<std::uint8_t>(data_ + offset::DATA_OFFSET) >> 4;
    }

    auto length() const
      -> std::uint16_t
    {
      return data_offset() * sizeof(std::uint32_t);
    }

    auto flags() const
      -> std::uint16_t
    {
      auto const data_offset_and_flags
        = detail::decode<std::uint16_t>(data_ + offset::DATA_OFFSET_AND_FLAGS);
      return data_offset_and_flags & 0x0fff;
    }

    auto window() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::WINDOW);
    }

    auto checksum() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::CHECKSUM);
    }

    auto urgent_pointer() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::URGENT_POINTER);
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, length());
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

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& function)
    {
      if (std::distance(first, last) < min_header_length) {
        return;
      }
      auto const header = tcp_header{first, last};
      if (function(header)
          && std::distance(first, last) >= header.length()) {
        function(header.payload());
      }
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  class udp_header
  {
  public:
    static constexpr std::uint8_t protocol = 17;
    static constexpr std::uint16_t header_length = 8;
    enum offset
    {
      SOURCE_PORT = 0,
      DESTINATION_PORT = SOURCE_PORT + 2,
      LENGTH = DESTINATION_PORT + 2,
      CHECKSUM = LENGTH + 2,
    };

    udp_header(
        std::uint8_t const* const data, std::uint8_t const* const last) noexcept
      : data_{data}, last_{last}
    {
    }

    auto source_port() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::SOURCE_PORT);
    }

    auto destination_port() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::DESTINATION_PORT);
    }

    auto length() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::LENGTH);
    }

    auto checksum() const
      -> std::uint16_t
    {
      return detail::decode<std::uint16_t>(data_ + offset::CHECKSUM);
    }

    auto next() const
      -> std::uint8_t const*
    {
      return std::next(data_, header_length);
    }

    auto end() const
      -> std::uint8_t const*
    {
      auto const length = std::size_t{this->length()};
      return length == 0 || data_length() < length
        ? last_
        : std::next(data_, length);
    }

    auto payload() const
      -> boost::iterator_range<std::uint8_t const*>
    {
      return boost::make_iterator_range(next(), end());
    }

    auto data_length() const noexcept
      -> std::size_t
    {
      return std::distance(data_, last_);
    }

    template <class Function>
    static void parse(
          std::uint8_t const* const first, std::uint8_t const* const last
        , Function& function)
    {
      if (std::distance(first, last) < header_length) {
        return;
      }
      auto const header = udp_header{first, last};
      if (function(header)) {
        function(header.payload());
      }
    }

  private:
    std::uint8_t const* data_;
    std::uint8_t const* last_;
  };

  template <class Function>
  inline void parse_icmpv6(
        std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f)
  {
    if (std::distance(first, last) < icmpv6::header_length) {
      return;
    }
    auto const header = icmpv6{first, last};
    switch (header.type()) {
    case icmpv6_neighbor_solicitation::icmp_type:
      if (std::distance(first, last)
          >= icmpv6_neighbor_solicitation::min_length) {
        f(icmpv6_neighbor_solicitation{header});
        return;
      }
      break;
    case icmpv6_neighbor_advertisement::icmp_type:
      if (std::distance(first, last)
          >= icmpv6_neighbor_advertisement::min_length) {
        f(icmpv6_neighbor_advertisement{header});
        return;
      }
      break;
    }
    f(header);
  }

  template <class Function>
  inline void parse_icmpv4(
        std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f)
  {
    if (std::distance(first, last) < icmpv4::header_length) {
      return;
    }
    auto const header = icmpv4{first, last};
    switch (header.type()) {
    case icmpv4_echo::request_type:
    case icmpv4_echo::reply_type:
      if (std::distance(first, last) >= icmpv4_echo::min_length) {
        f(icmpv4_echo(header));
        return;
      }
      break;
    case 3: // destination unreachable
      // TODO
      break;
    case 4: // source quench
      // TODO
      break;
    case icmpv4_redirect::icmp_type:
      if (std::distance(first, last) >= icmpv4_redirect::min_length) {
        f(icmpv4_redirect{header});
        return;
      }
      break;
    case 9: case 10:
      // TODO
      break;
    case 11: // time exceeded
      // TODO
      break;
    case 12: // parameter problem
      // TODO
      break;
    case icmpv4_timestamp::request_type:
    case icmpv4_timestamp::reply_type:
      if (std::distance(first, last) >= icmpv4_timestamp::min_length) {
        f(icmpv4_timestamp{header});
        return;
      }
      break;
    case 15: // information request
    case 16: // information reply
      // TODO
      break;
    case icmpv4_address_mask::request_type:
    case icmpv4_address_mask::reply_type:
      if (std::distance(first, last) >= icmpv4_address_mask::min_length) {
        f(icmpv4_address_mask{header});
        return;
      }
      break;
    }
    f(header);
  }

  template <class Function>
  inline void parse_ipv6_hop_by_hop(
        std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f)
  {
    if (std::distance(first, last) < ipv6_extension_header::min_header_length) {
      return;
    }
    auto const header = ipv6_extension_header{first, last};
    if (!f(header) || std::distance(first, last) < header.length()) {
      return;
    }
    ip_next_header(header.next_header(), header.next(), header.end(), f);
  }

  template <class Function>
  inline void ip_next_header(
        std::uint8_t const protocol
      , std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f)
  {
    switch (protocol) {
    case 0:
      parse_ipv6_hop_by_hop(first, last, f);
      return;
    case icmpv4::protocol:
      parse_icmpv4(first, last, f);
      return;
    case tcp_header::protocol:
      tcp_header::parse(first, last, f);
      return;
    case udp_header::protocol:
      udp_header::parse(first, last, f);
      return;
      // case 41: return;
    case 43:
      // parse_ipv6_router(first, last, f);
      parse_ipv6_hop_by_hop(first, last, f);
      return;
    case ipv6_fragment_header::protocol:
      ipv6_fragment_header::parse(first, last, f);
      return;
    case ah_header::protocol:
      ah_header::parse(first, last, f);
      return;
    case icmpv6::protocol:
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
  inline void ether_next_header(
        std::uint16_t const type
      , std::uint8_t const* const first, std::uint8_t const* const last
      , Function& f)
  {
    switch (type) {
    case ipv4_header::type:
      ipv4_header::parse(first, last, f);
      return;
    case arp::type:
      arp::parse(first, last, f);
      return;
    case vlan_tag::type:
      vlan_tag::parse(first, last, f);
      return;
    case ipv6_header::type:
      ipv6_header::parse(first, last, f);
      return;
    case lldpdu::type:
      lldpdu::parse(first, last, f);
      return;
    default:
      f(boost::make_iterator_range(first, last));
      return;
    }
  }

  template <class Range, class Function>
  inline void for_each_header(Range const& frame, Function f)
  {
    ether_header::parse(frame.data(), frame.data() + frame.size(), f);
  }

  namespace packet_parser_detail {

    template <class Function, class T>
    struct invoker
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

  } // namespace packet_parser_detail

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
      auto func = packet_parser_detail::invoker<Function, canard::ether_header>{
        std::forward<Function>(function)
      };
      ether_header::parse(first_, last_, func);
      return *this;
    }

    template <class Function>
    auto lldpdu(Function&& function) const
      -> packet const&
    {
      auto func = packet_parser_detail::invoker<Function, canard::lldpdu>{
        std::forward<Function>(function)
      };
      ether_header::parse(first_, last_, func);
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
