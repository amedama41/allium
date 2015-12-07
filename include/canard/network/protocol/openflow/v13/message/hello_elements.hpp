#ifndef CANARD_NETWORK_OPENFLOW_V13_HELLO_ELEM_VERSIONBITMAP_HPP
#define CANARD_NETWORK_OPENFLOW_V13_HELLO_ELEM_VERSIONBITMAP_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/endian/conversion.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/irange.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace hello_elements {

        class versionbitmap
        {
        public:
            static protocol::ofp_hello_elem_type const hello_element_type
                = protocol::OFPHET_VERSIONBITMAP;

            explicit versionbitmap(std::vector<std::uint32_t> bitmaps)
                : versionbitmap_{
                      hello_element_type
                    , std::uint16_t(sizeof(versionbitmap_) + bitmaps.size() * sizeof(bitmaps[0]))
                  }
                , bitmaps_{std::move(bitmaps)}
            {
                using boost::adaptors::reversed;
                auto const it = boost::find_if(
                          bitmaps_ | reversed
                        , [](std::uint32_t const bitmap) { return bitmap != 0; });
                bitmaps_.erase(it.base(), bitmaps_.end());
                if (bitmaps_.empty()) {
                    throw std::runtime_error{"versionbitmap nust not be empty"};
                }
            }

            auto type() const
                -> protocol::ofp_hello_elem_type
            {
                return protocol::ofp_hello_elem_type(versionbitmap_.type);
            }

            auto length() const
                -> std::uint16_t
            {
                return versionbitmap_.length;
            }

            auto support(std::uint8_t const version) const
                -> bool
            {
                auto const bitmap_bits = std::size_t(32);
                auto const array_index = std::size_t(version / bitmap_bits);
                auto const bit_index = std::size_t(version % bitmap_bits);
                if (bitmaps_.size() < array_index) {
                    return false;
                }
                return (std::uint32_t(1) << bit_index) & bitmaps_[array_index];
            }

            auto max_support_version() const
                -> std::uint8_t
            {
                auto bit_index = std::size_t{0};
                auto bitmap = bitmaps_.back();
                while (bitmap != 0) {
                    bitmap >>= 1;
                    ++bit_index;
                }
                return bit_index - 1 + (bitmaps_.size() - 1) * 32;
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                using boost::adaptors::transformed;
                v13_detail::encode(container, versionbitmap_);
                boost::for_each(bitmaps_, [&](std::uint32_t const bitmap) {
                    v13_detail::encode(container, bitmap);
                });
                return boost::push_back(container
                        , boost::irange<std::size_t>(0, v13_detail::padding_length(length())) | transformed([](std::size_t){ return 0; }));
            }

        private:
            versionbitmap(v13_detail::ofp_hello_elem_versionbitmap const& versionbitmap, std::vector<std::uint32_t> bitmaps)
                : versionbitmap_(versionbitmap)
                , bitmaps_(std::move(bitmaps))
            {
                if (versionbitmap_.type != hello_element_type) {
                    throw 1;
                }
                if (versionbitmap_.length != sizeof(versionbitmap_) + bitmaps_.size() * sizeof(std::uint32_t)) {
                    throw 1;
                }
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> versionbitmap
            {
                auto const vbitmap = v13_detail::decode<v13_detail::ofp_hello_elem_versionbitmap>(first, last);

                auto bitmaps = std::vector<std::uint32_t>((vbitmap.length - sizeof(vbitmap)) / sizeof(std::uint32_t));
                std::copy_n(first, bitmaps.size() * sizeof(std::uint32_t)
                        , reinterpret_cast<unsigned char*>(&bitmaps[0]));
                std::advance(first, bitmaps.size() * sizeof(std::uint32_t));
                boost::for_each(bitmaps, [](std::uint32_t& bitmap) {
                    boost::endian::big_to_native_inplace(bitmap);
                });

                std::advance(first, v13_detail::padding_length(vbitmap.length));

                return versionbitmap{vbitmap, std::move(bitmaps)};
            }

        private:
            v13_detail::ofp_hello_elem_versionbitmap versionbitmap_;
            std::vector<std::uint32_t> bitmaps_;
        };


        class unknown_element
        {
        public:
            explicit unknown_element(std::uint16_t const type)
                : header_{type, sizeof(header_)}
            {
            }

            auto type() const
                -> protocol::ofp_hello_elem_type
            {
                return protocol::ofp_hello_elem_type(header_.type);
            }

            auto length() const
                -> std::uint16_t
            {
                return sizeof(header_) + data_.size();
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                v13_detail::encode(container, header_);
                return boost::push_back(container, data_);
            }

        public:
            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> unknown_element
            {
                auto const header = v13_detail::decode<v13_detail::ofp_hello_elem_header>(first, last);

                auto data = std::vector<unsigned char>(header.length - sizeof(header));
                std::copy_n(first, data.size(), data.begin());
                std::advance(first, data.size());

                std::advance(first, v13_detail::padding_length(header.length));

                return unknown_element(header, std::move(data));
            }

        private:
            unknown_element(v13_detail::ofp_hello_elem_header const& header, std::vector<unsigned char> data)
                : header_(header)
                , data_(std::move(data))
            {
                if (header.length != sizeof(header) + data.size()) {
                    throw 1;
                }
            }

        private:
            v13_detail::ofp_hello_elem_header header_;
            std::vector<unsigned char> data_;
        };

    } // namespace hello_elements

    using hello_element_list = std::tuple<
          hello_elements::versionbitmap
        , hello_elements::unknown_element
    >;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_HELLO_ELEM_VERSIONBITMAP_HPP
