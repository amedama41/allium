#ifndef CANARD_NABE_LLDP_HPP
#define CANARD_NABE_LLDP_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/endian/conversion.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/size.hpp>
#include <canard/nabe/ether_header.hpp>

namespace canard {
namespace nabe {

  namespace lldptlv {

    namespace lldptlv_detail {

      template <class Container, class T>
      auto encode(Container& container, T t)
        -> Container&
      {
        boost::endian::big_to_native_inplace(t);
        std::uint8_t buffer[sizeof(t)];
        std::memcpy(buffer, &t, sizeof(buffer));
        return boost::push_back(container, buffer);
      }

    } // namespace lldptlv_detail

    inline constexpr auto tlv_header(
        std::uint8_t const tlv_type, std::uint16_t const tlv_info_length)
      -> std::uint16_t
    {
      return (std::uint16_t{tlv_type} << 9) | (tlv_info_length & 0x1FF);
    }

    class chassis_id
    {
    public:
      static std::uint8_t const tlv_type = 1;

      chassis_id(std::uint8_t const subtype, std::string chassis_id)
        : subtype_(subtype)
        , chassis_id_(std::move(chassis_id))
      {
      }

      auto tlv_header() const
        -> std::uint16_t
      {
        return lldptlv::tlv_header(tlv_type, tlv_info_length());
      }

      auto tlv_info_length() const
        -> std::uint16_t
      {
        return sizeof(subtype_) + chassis_id_.size();
      }

      template <class Container>
      auto encode(Container& container) const
        -> Container&
      {
        lldptlv_detail::encode(container, tlv_header());
        return boost::push_back(
            lldptlv_detail::encode(container, subtype_), chassis_id_);
      }

    private:
      std::uint8_t subtype_;
      std::string chassis_id_;
    };

    class port_id
    {
    public:
      static std::uint8_t const tlv_type = 2;

      port_id(std::uint8_t const subtype, std::string port_id)
        : subtype_(subtype)
        , port_id_(std::move(port_id))
      {
      }

      auto tlv_header() const
        -> std::uint16_t
      {
        return lldptlv::tlv_header(tlv_type, tlv_info_length());
      }

      auto tlv_info_length() const
        -> std::uint16_t
      {
        return sizeof(subtype_) + port_id_.size();
      }

      template <class Container>
      auto encode(Container& container) const
        -> Container&
      {
        lldptlv_detail::encode(container, tlv_header());
        return boost::push_back(
            lldptlv_detail::encode(container, subtype_), port_id_);
      }

    private:
      std::uint8_t subtype_;
      std::string port_id_;
    };

    class time_to_live
    {
    public:
      static std::uint8_t const tlv_type = 3;

      explicit time_to_live(std::uint16_t const time_to_live)
        : time_to_live_(time_to_live)
      {
      }

      static constexpr auto tlv_header()
        -> std::uint16_t
      {
        return lldptlv::tlv_header(tlv_type, tlv_info_length());
      }

      static constexpr auto tlv_info_length()
        -> std::uint16_t
      {
        return sizeof(time_to_live_);
      }

      template <class Container>
      auto encode(Container& container) const
        -> Container&
      {
        lldptlv_detail::encode(container, tlv_header());
        return lldptlv_detail::encode(container, time_to_live_);
      }

    private:
      std::uint16_t time_to_live_;
    };

    class end_of_lldpdu
    {
    public:
      static std::uint8_t const tlv_type = 0;

      auto tlv_header() const
        -> std::uint16_t
      {
        return lldptlv::tlv_header(0, 0);
      }

      template <class Container>
      auto encode(Container& container) const
        -> Container&
      {
        return lldptlv_detail::encode(container, tlv_header());
      }
    };

  } // namespace lldptlv

  namespace detail {

    template <class Container>
    class encoder
    {
    public:
      explicit encoder(Container& container)
        : container_(container)
      {
      }

      template <class T>
      auto operator()(T const& t) const
        -> Container&
      {
        return t.encode(container_);
      }

    private:
      Container& container_;
    };

    template <class Container>
    inline auto make_encoder(Container& container)
      -> encoder<Container>
    {
      return encoder<Container>{container};
    }

  } // namespace detail

  template <class... LLDPTLVs>
  class lldpdu
  {
  public:
    static std::uint16_t const ether_type = 0x88CC;

    lldpdu(lldptlv::chassis_id chassis_id
         , lldptlv::port_id port_id
         , lldptlv::time_to_live time_to_live
         , LLDPTLVs... lldp_tlvs)
      : lldp_tlvs_(
            std::move(chassis_id)
          , std::move(port_id)
          , std::move(time_to_live)
          , std::move(lldp_tlvs)...)
    {
    }

    template <class Container>
    auto encode(Container& container) const
      -> Container&
    {
      boost::fusion::for_each(lldp_tlvs_, detail::make_encoder(container));
      return lldptlv::end_of_lldpdu{}.encode(container);
    }

  private:
    std::tuple<
        lldptlv::chassis_id
      , lldptlv::port_id
      , lldptlv::time_to_live
      , LLDPTLVs...
    > lldp_tlvs_;
  };

  template <class... LLDPTLVs>
  auto lldp(lldptlv::chassis_id chassis_id
          , lldptlv::port_id port_id
          , lldptlv::time_to_live time_to_live
          , LLDPTLVs&&... lldp_tlvs)
    -> lldpdu<typename std::decay<LLDPTLVs>::type...>
  {
    return {
        std::move(chassis_id), std::move(port_id), std::move(time_to_live)
      , std::forward<LLDPTLVs>(lldp_tlvs)...
    };
  }

  template <class... LLDPTLVs>
  auto operator<<(ether_header eth_header, lldpdu<LLDPTLVs...> lldp)
    -> std::tuple<ether_header, lldpdu<LLDPTLVs...>>
  {
    eth_header.ether_type(lldp.ether_type);
    return std::make_tuple(std::move(eth_header), std::move(lldp));
  }

  template <class... PDUs, class Container>
  auto encode(
      std::tuple<ether_header, PDUs...> const& packet, Container& container)
    -> Container&
  {
    boost::fusion::for_each(packet, detail::make_encoder(container));
    constexpr std::uint8_t padding[64] = {0};
    auto const padding_size = std::min<std::size_t>(boost::size(container), 64);
    return boost::push_back(
        container, boost::make_iterator_range_n(padding, padding_size));
  }

} // namespace nabe
} // namespace canard

#endif // CANARD_NABE_LLDP_HPP

