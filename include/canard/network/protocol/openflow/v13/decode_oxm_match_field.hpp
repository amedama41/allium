#ifndef CANARD_NETWORK_OPENFLOW_V13_DECODE_OXM_MATCH_FIELD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DECODE_OXM_MATCH_FIELD_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <boost/format.hpp>
#include <canard/as_byte_range.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_field_list.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    template <class ReturnType, class Iterator, class Func>
    auto decode_oxm_match_field(Iterator& first, Iterator last, Func func)
        -> ReturnType
    {
        static_assert(std::tuple_size<default_oxm_match_field_list>::value == 40, "");

        auto oxm_header = std::uint32_t{};
        std::copy_n(first, sizeof(oxm_header), canard::as_byte_range(oxm_header).begin());
        oxm_header = v13_detail::ntoh(oxm_header);

        switch (oxm_header >> 9) {
#       define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODE_CASE(z, N, _) \
        case std::tuple_element<N, default_oxm_match_field_list>::type::oxm_type(): \
            return func(std::tuple_element<N, default_oxm_match_field_list>::type::decode(first, last)); \
            break;
        BOOST_PP_REPEAT(40, CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODE_CASE, _)
#       undef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_FIELD_DECODE_CASE

        default:
            // TODO
            std::advance(first, sizeof(oxm_header) + (oxm_header & 0x0000000f));
            throw std::runtime_error{(boost::format{"%1%: oxm_type(%2%) is unknwon"} % __func__ % (oxm_header >> 9)).str()};
            // return func(any_oxm_match_field{});
        }
    }

    namespace v13_detail {

        struct to_any_oxm_match_field
        {
            template <class OXMMatchField>
            auto operator()(OXMMatchField&& oxm_match_field) const
                -> any_oxm_match_field
            {
                return any_oxm_match_field{std::forward<OXMMatchField>(oxm_match_field)};
            }
        };

    } // namespace v13_detail

    template <class Iterator>
    auto decode_oxm_match_field(Iterator& first, Iterator last)
        -> any_oxm_match_field
    {
        return decode_oxm_match_field<any_oxm_match_field>(first, last, v13_detail::to_any_oxm_match_field{});
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DECODE_OXM_MATCH_FIELD_HPP
