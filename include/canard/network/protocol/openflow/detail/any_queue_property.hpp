#ifndef CANARD_NETWORK_OPENFLOW_DETAIL_ANY_QUEUE_PROPERTY_HPP
#define CANARD_NETWORK_OPENFLOW_DETAIL_ANY_QUEUE_PROPERTY_HPP

#include <cstdint>
#include <memory>
#include <utility>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <canard/network/protocol/openflow/detail/min_base_size_element.hpp>
#include <canard/network/protocol/openflow/detail/visitors.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace detail {

    template <class QueuePropertyList, class QueuePropertyDecoder>
    class any_queue_property
    {
        using queue_property_variant
            = typename boost::make_variant_over<QueuePropertyList>::type;

    public:
        static constexpr std::size_t min_base_size
            = detail::min_base_size_element<QueuePropertyList>::value;

        template <
              class QueueProperty
            , typename std::enable_if<
                  !canard::is_related<any_queue_property, QueueProperty>::value
              >::type* = nullptr
        >
        explicit any_queue_property(QueueProperty&& property)
            : variant_(std::forward<QueueProperty>(property))
        {
        }

        template <
              class QueueProperty
            , typename std::enable_if<
                  !canard::is_related<any_queue_property, QueueProperty>::value
              >::type* = nullptr
        >
        auto operator=(QueueProperty&& property)
            -> any_queue_property&
        {
            variant_ = std::forward<QueueProperty>(property);
            return *this;
        }

        auto property() const noexcept
            -> std::uint16_t
        {
            auto visitor = detail::property_visitor{};
            return boost::apply_visitor(visitor, variant_);
        }

        auto length() const noexcept
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
            return QueuePropertyDecoder::template decode<any_queue_property>(
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
                return any_queue_property{
                    std::forward<QueueProperty>(property)
                };
            }
        };

    private:
        queue_property_variant variant_;
    };

    template <class T, class QueuePropertyList, class QueuePropertyDecoder>
    auto any_cast(
            any_queue_property<QueuePropertyList, QueuePropertyDecoder> const& property)
        -> T const&
    {
        return boost::get<T>(property.variant_);
    }

    template <class T, class QueuePropertyList, class QueuePropertyDecoder>
    auto any_cast(
            any_queue_property<QueuePropertyList, QueuePropertyDecoder> const* const property)
        -> T const*
    {
        return boost::get<T>(std::addressof(property->variant_));
    }

} // namespace detail
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_DETAIL_ANY_QUEUE_PROPERTY_HPP
