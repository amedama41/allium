#ifndef CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP

#include <cstdint>
#include <memory>
#include <boost/variant/variant.hpp>
#include <canard/network/protocol/openflow/detail/min_raw_size.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v10/detail/decode_queue_property.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>
#include <canard/network/protocol/openflow/v10/queue_properties.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class any_queue_property
    {
        using queue_property_variant = boost::make_variant_over<
            default_queue_property_list
        >::type;

    public:
        static std::uint16_t const min_raw_size
            = detail::raw_size_min_element<default_queue_property_list >::value;

        template <
              class QueueProperty
            , typename std::enable_if<!canard::is_related<any_queue_property, QueueProperty>::value>::type* = nullptr
        >
        explicit any_queue_property(QueueProperty&& property)
            : variant_(std::move(property))
        {
        }

        template <
              class QueueProperty
            , typename std::enable_if<!canard::is_related<any_queue_property, QueueProperty>::value>::type* = nullptr
        >
        auto operator=(QueueProperty&& property)
            -> any_queue_property&
        {
            variant_ = std::forward<QueueProperty>(property);
            return *this;
        }

        auto property() const
            -> std::uint16_t
        {
            auto visitor = detail::property_visitor{};
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
            -> any_queue_property
        {
            return v10_detail::decode_queue_property<any_queue_property>(
                first, last, to_any_queue_property{});
        }

        template <class T>
        friend auto any_cast(any_queue_property const&)
            -> T const&;

        template <class T>
        friend auto any_cast(any_queue_property const*)
            -> T const*;

    private:
        struct to_any_queue_property
        {
            template <class QueueProperty>
            auto operator()(QueueProperty&& property) const
                -> any_queue_property
            {
                return any_queue_property{std::forward<QueueProperty>(property)};
            }
        };

    private:
        queue_property_variant variant_;
    };

    template <class T>
    auto any_cast(any_queue_property const& property)
        -> T const&
    {
        return boost::get<T>(property.variant_);
    }

    template <class T>
    auto any_cast(any_queue_property const* const property)
        -> T const*
    {
        return boost::get<T>(std::addressof(property->variant_));
    }

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_ANY_QUEUE_PROPEPRTY_HPP
