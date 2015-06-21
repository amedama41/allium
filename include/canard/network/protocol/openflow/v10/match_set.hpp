#ifndef CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP

#include <type_traits>
#include <utility>
#include <canard/network/protocol/openflow/v10/detail/match_adaptor.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    struct match_set
        : public v10_detail::match_adaptor<match_set>
    {
    public:
        template <class... MatchFields
            , typename std::enable_if<
                       !canard::is_related<match_set, MatchFields...>::value
                    && !canard::is_related<v10_detail::ofp_match, MatchFields...>::value
              >::type* = nullptr>
        match_set(MatchFields&&... fields)
            : match_{protocol::OFPFW_ALL, 0}
        {
            add_impl(std::forward<MatchFields>(fields)...);
        }

        explicit match_set(v10_detail::ofp_match const& match)
            : match_(match)
        {
        }

        template <class MatchField>
        void add(MatchField&& field)
        {
            field.set_value(match_);
        }

        auto ofp_match() const
            -> v10_detail::ofp_match const&
        {
            return match_;
        }

    private:
        void add_impl()
        {
        }

        template <class MatchField, class... MatchFields>
        void add_impl(MatchField&& field, MatchFields&&... fields)
        {
            add(std::forward<MatchField>(field));
            add_impl(std::forward<MatchFields>(fields)...);
        }

    private:
        v10_detail::ofp_match match_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MATCH_SET_HPP
