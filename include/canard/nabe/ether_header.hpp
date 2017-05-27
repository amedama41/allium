#ifndef CANARD_NABE_ETHER_HEADER_HPP
#define CANARD_NABE_ETHER_HEADER_HPP

#include <cstdint>
#include <boost/endian/conversion.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/mac_address.hpp>

namespace canard {
namespace nabe {

  class ether_header
  {
  public:
    ether_header(mac_address destination, mac_address source)
      : destination_(std::move(destination))
      , source_(std::move(source))
    {
    }

    void ether_type(std::uint16_t const ether_type)
    {
      ether_type_ = ether_type;
    }

    template <class Container>
    auto encode(Container& container) const
      -> Container&
    {
      boost::push_back(container, destination_.to_bytes());
      boost::push_back(container, source_.to_bytes());
      return boost::push_back(
            container
          , canard::as_byte_range(boost::endian::native_to_big(ether_type_)));
    }

  private:
    mac_address destination_;
    mac_address source_;
    std::uint16_t ether_type_;
  };

} // namespace nabe
} // namespace canard

#endif // CANARD_NABE_ETHER_HEADER_HPP
