#ifndef CANARD_NETWORK_OPENFLOW_V13_ANY_HELLO_ELEMENT_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ANY_HELLO_ELEMENT_HPP

#include <cstdint>
#include <type_traits>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <canard/network/protocol/openflow/v13/decode_hello_element.hpp>
#include <canard/network/protocol/openflow/v13/detail/construct.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class any_hello_element
    {
        using any_hello_element_variant = boost::make_variant_over<
            hello_element_list
        >::type;

    public:
        template <class HelloElem, typename std::enable_if<!is_related<HelloElem, any_hello_element>::value>::type* = nullptr>
        any_hello_element(HelloElem&& hello_elem)
            : variant_(std::forward<HelloElem>(hello_elem))
        {
        }

        auto type() const
            -> ofp_hello_elem_type
        {
            auto visitor = detail::type_visitor<ofp_hello_elem_type>{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto length() const
            -> std::uint16_t
        {
            auto visitor = detail::length_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto visitor = detail::encoding_visitor<Container>{container};
            return boost::apply_visitor(visitor, variant_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> any_hello_element
        {
            return detail::decode_hello_element<any_hello_element>(
                    first, last, detail::construct<any_hello_element>{});
        }

        template <class T>
        friend auto any_cast(any_hello_element const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_hello_element const*)
            -> T const*;

    private:
        any_hello_element_variant variant_;
    };

    template <class T>
    auto any_cast(any_hello_element const& hello_elem)
        -> T const&
    {
        return boost::get<T>(hello_elem.variant_);
    }

    template <class T>
    auto any_cast(any_hello_element const* const hello_elem)
        -> T const*
    {
        return boost::get<T>(&hello_elem->variant_);
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ANY_HELLO_ELEMENT_HPP
