#ifndef CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP

#include <cstdint>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {
namespace v13 {

#define DEFINE_OXM_VISITOR(FUNC_NAME, RETURN_TYPE) \
        class FUNC_NAME ## _visitor \
            : public boost::static_visitor<RETURN_TYPE> \
        { \
        public: \
            template <class T> \
            auto operator()(T const& t) const noexcept \
                -> RETURN_TYPE \
            { \
                return t.FUNC_NAME(); \
            } \
        }; \

        DEFINE_OXM_VISITOR(oxm_class, openflow::v13::protocol::ofp_oxm_class)
        DEFINE_OXM_VISITOR(oxm_field, std::uint8_t)
        DEFINE_OXM_VISITOR(oxm_type, std::uint32_t)
        DEFINE_OXM_VISITOR(oxm_header, std::uint32_t)
        DEFINE_OXM_VISITOR(oxm_has_mask, bool)
        DEFINE_OXM_VISITOR(oxm_length, std::uint8_t)
        DEFINE_OXM_VISITOR(is_wildcard, bool)
        DEFINE_OXM_VISITOR(is_exact, bool)

#undef DEFINE_OXM_VISITOR

} // namespace v13
} // namespace detail

namespace v13 {

    namespace v13_detail {

        class calculating_exact_length_visitor
            : public boost::static_visitor<std::uint16_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint16_t
            {
                return v13_detail::exact_length(t.length());
            }
        };

        class instruction_order_visitor
            : public boost::static_visitor<std::uint64_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint64_t
            {
                return instruction_order(t);
            }
        };

        using detail::v13::oxm_type_visitor;
        using detail::v13::oxm_header_visitor;
        using detail::v13::oxm_has_mask_visitor;
        using detail::v13::oxm_length_visitor;

    } // namespace v13_detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP
