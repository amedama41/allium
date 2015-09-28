#ifndef CANARD_NETWORK_OPENFLOW_V13_DECODE_HELLO_ELEMENT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DECODE_HELLO_ELEMENT_HPP

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/endian/conversion.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace v13_detail {

        template <class Iterator>
        inline auto parse_hello_elem_header(Iterator first, Iterator last)
            -> v13_detail::ofp_hello_elem_header
        {
            if (std::distance(first, last) < sizeof(v13_detail::ofp_hello_elem_header)) {
                throw std::runtime_error{"too short message"};
            }
            auto header = v13_detail::ofp_hello_elem_header{};
            std::copy_n(first, sizeof(header), canard::as_byte_range(header).begin());
            boost::endian::big_to_native_inplace(header);
            return header;
        }

        template <class ReturnType, class Iterator, class Func>
        auto decode_hello_element(Iterator& first, Iterator last, Func&& func)
            -> ReturnType
        {
            auto const header = v13_detail::parse_hello_elem_header(first, last);
            if (header.length > std::distance(first, last)) {
                throw std::runtime_error{"invalid length"};
            }
            switch (header.type) {
            static_assert(std::tuple_size<hello_element_list>::value - 1 == 1, "");
#define     CANARD_NETWORK_OPENFLOW_DECODE_HELLO_ELEMENT_CASE(z, N, _) \
            case std::tuple_element<N, hello_element_list>::type::hello_element_type: \
                return func(std::tuple_element<N, hello_element_list>::type::decode(first, last));
            BOOST_PP_REPEAT(1, CANARD_NETWORK_OPENFLOW_DECODE_HELLO_ELEMENT_CASE, _)
#undef      CANARD_NETWORK_OPENFLOW_DECODE_HELLO_ELEMENT_CASE
            default:
                return func(hello_elements::unknown_element::decode(first, last));
            }
        }

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DECODE_HELLO_ELEMENT_HPP
