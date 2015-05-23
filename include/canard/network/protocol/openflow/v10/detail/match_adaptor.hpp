#ifndef CANARD_NETWORK_OPENFLOW_V10_MATCH_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MATCH_ADAPTOR_HPP

#include <cstdint>
#include <type_traits>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace v10_detail {

    namespace match_adaptor_detail {

        template <class MatchField, typename std::enable_if<MatchField::has_mask::value>::type* = nullptr>
        auto get_field(v10_detail::ofp_match const& match)
            -> MatchField
        {
            return MatchField{
                  MatchField::field_value(match)
                , MatchField::field_cidr_suffix(match)
            };
        }

        template <class MatchField, typename std::enable_if<!MatchField::has_mask::value>::type* = nullptr>
        auto get_field(v10_detail::ofp_match const& match)
            -> MatchField
        {
            return MatchField{
                MatchField::field_value(match)
            };
        }

    } // namespace match_adaptor_detail

    template <class T>
    class match_adaptor
    {
    public:
        template <class MatchField>
        auto get() const
            -> MatchField
        {
            return match_adaptor_detail::get_field<MatchField>(base_ofp_match());
        }

    private:
        auto base_ofp_match() const
            -> v10_detail::ofp_match const&
        {
            return static_cast<T const*>(this)->ofp_match();
        }
    };

} // namespace v10_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MATCH_ADAPTOR_HPP
