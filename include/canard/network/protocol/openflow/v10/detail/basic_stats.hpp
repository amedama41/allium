#ifndef CANARD_NETWORK_OPENFLOW_V10_BAISC_STATS_HPP
#define CANARD_NETWORK_OPENFLOW_V10_BAISC_STATS_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace messages {
namespace statistics {
namespace stats_detail {

    template <class StatsType>
    struct stats_message_type;

    template <>
    struct stats_message_type<v10_detail::ofp_stats_request>
    {
        static constexpr protocol::ofp_type value
            = protocol::OFPT_STATS_REQUEST;
    };

    template <>
    struct stats_message_type<v10_detail::ofp_stats_reply>
    {
        static constexpr protocol::ofp_type value
            = protocol::OFPT_STATS_REPLY;
    };

    template <class T, class StatsType>
    class basic_stats
        : public v10_detail::basic_openflow_message<T>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = stats_message_type<StatsType>::value;

        auto header() const noexcept
            -> v10_detail::ofp_header const&
        {
            return static_cast<T const*>(this)->stats().header;
        }

        auto stats_type() const noexcept
            -> protocol::ofp_stats_types
        {
            return T::stats_type_value;
        }

        auto flags() const noexcept
            -> std::uint16_t
        {
            return static_cast<T const*>(this)->stats().flags;
        }

        static void validate(v10_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < sizeof(StatsType)) {
                throw std::runtime_error{"invalid length"};
            }
        }
    };


    template <class T, class StatsType>
    class empty_body_stats
        : public basic_stats<T, StatsType>
    {
    public:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, stats_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            return T{detail::decode<StatsType>(first, last)};
        }

        using basic_stats<T, StatsType>::validate;

        static void validate(StatsType const& stats)
        {
            if (stats.type != T::stats_type_value) {
                throw std::runtime_error{"invalid stats type"};
            }
            if (stats.header.length != sizeof(StatsType)) {
                throw std::runtime_error{"invalid stats length"};
            }
        }

    protected:
        empty_body_stats(
                std::uint16_t const flags, std::uint32_t const xid) noexcept
            : stats_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , sizeof(StatsType)
                    , xid
                  }
                , T::stats_type_value
                , flags
              }
        {
        }

        explicit empty_body_stats(StatsType const& stats) noexcept
            : stats_(stats)
        {
        }

    private:
        friend basic_stats<T, StatsType>;

        auto stats() const noexcept
            -> StatsType const&
        {
            return stats_;
        }

    private:
        StatsType stats_;
    };


    template <class T, class BodyType, class StatsType>
    class single_element_stats
        : public basic_stats<T, StatsType>
    {
    public:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, stats_);
            return detail::encode(container, body_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const stats = detail::decode<StatsType>(first, last);
            return T{stats, detail::decode<BodyType>(first, last)};
        }

        using basic_stats<T, StatsType>::validate;

        static void validate(StatsType const& stats)
        {
            if (stats.type != T::stats_type_value) {
                throw std::runtime_error{"invalid stats type"};
            }
            if (stats.header.length
                    != sizeof(StatsType) + sizeof(BodyType)) {
                throw std::runtime_error{"invalid stats length"};
            }
        }

    protected:
        single_element_stats(
                  std::uint16_t const flags
                , BodyType const& body
                , std::uint32_t const xid) noexcept
            : stats_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , sizeof(StatsType) + sizeof(BodyType)
                    , xid
                  }
                , T::stats_type_value
                , flags
              }
            , body_(body)
        {
        }

        single_element_stats(
                StatsType const& stats, BodyType const& body) noexcept
            : stats_(stats)
            , body_(body)
        {
        }

        auto body() const noexcept
            -> BodyType const&
        {
            return body_;
        }

    private:
        friend basic_stats<T, StatsType>;

        auto stats() const noexcept
            -> StatsType const&
        {
            return stats_;
        }

    private:
        StatsType stats_;
        BodyType body_;
    };


    template <class T, class BodyType, class StatsType>
    class array_body_stats
        : public basic_stats<T, StatsType>
    {
        using elem_type = typename std::remove_all_extents<BodyType>::type;

    public:
        using body_type = std::vector<elem_type>;
        using iterator = typename body_type::iterator;
        using const_iterator = typename body_type::const_iterator;

        auto size() const noexcept
            -> std::size_t
        {
            return body_.size();
        }

        auto begin() noexcept
            -> iterator
        {
            return body_.begin();
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return body_.begin();
        }

        auto end() noexcept
            -> iterator
        {
            return body_.end();
        }

        auto end() const noexcept
            -> const_iterator
        {
            return body_.end();
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, stats_);
            boost::for_each(
                      body_
                    , [&](elem_type const& body) { body.encode(container); });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const stats = detail::decode<StatsType>(first, last);
            auto const body_length
                = stats.header.length - sizeof(StatsType);
            last = std::next(first, body_length);

            auto body = body_type{};
            body.reserve(body_length / elem_type::base_size);
            while (std::distance(first, last) >= elem_type::base_size) {
                body.push_back(elem_type::decode(first, last));
            }

            if (first != last) {
                throw std::runtime_error{"invalid stats length"};
            }
            return T{stats, std::move(body)};
        }

        using basic_stats<T, StatsType>::validate;

        static void validate(StatsType const& stats)
        {
            if (stats.type != T::stats_type_value) {
                throw std::runtime_error{"invalid stats type"};
            }
            if (stats.header.length < sizeof(StatsType)) {
                throw std::runtime_error{"invalid stats length"};
            }
        }

    protected:
        array_body_stats(
                  std::uint16_t const flags
                , body_type&& body
                , std::uint32_t const xid)
            : stats_{
                  v10_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , std::uint16_t(
                            sizeof(StatsType) + calc_body_length(body))
                    , xid
                  }
                , T::stats_type_value
                , flags
              }
            , body_(std::move(body))
        {
        }

        array_body_stats(
                StatsType const& stats, body_type&& body)
            : stats_(stats)
            , body_(std::move(body))
        {
        }

        array_body_stats(array_body_stats const&)
            = default;

        array_body_stats(array_body_stats&& other)
            : stats_(other.stats_)
            , body_(std::move(other).body_)
        {
            other.stats_.header.length = sizeof(StatsType);
        }

        auto operator=(array_body_stats const&)
            -> array_body_stats& = default;

        auto operator=(array_body_stats&& other)
            -> array_body_stats&
        {
            auto tmp = std::move(other);
            std::swap(stats_, tmp.stats_);
            body_.swap(tmp.body_);
            return *this;
        }

    private:
        friend basic_stats<T, StatsType>;

        auto stats() const noexcept
            -> StatsType const&
        {
            return stats_;
        }

        static auto calc_body_length(body_type const& body)
            -> std::uint16_t
        {
            auto const body_length = boost::accumulate(
                      body, std::size_t{0}
                    , [](std::size_t const sum, elem_type const& e) {
                            return sum + e.length();
                      });
            if (body_length + sizeof(StatsType)
                    > std::numeric_limits<std::uint16_t>::max()) {
                throw std::runtime_error{"body size is too big"};
            }
            return std::uint16_t(body_length);
        }

    private:
        StatsType stats_;
        body_type body_;
    };


    template <class StatsType, class T, class BodyType>
    using basic_stats_t = typename std::conditional<
          std::is_same<BodyType, void>::value
        , empty_body_stats<T, StatsType>
        , typename std::conditional<
              std::is_array<BodyType>::value
            , array_body_stats<T, BodyType, StatsType>
            , single_element_stats<T, BodyType, StatsType>
          >::type
    >::type;


    template <class T, class BodyType>
    class basic_stats_request
        : public basic_stats_t<v10_detail::ofp_stats_request, T, BodyType>
    {
    protected:
        using base_type
            = basic_stats_t<v10_detail::ofp_stats_request, T, BodyType>;

    public:
        using base_type::base_type;
    };


    template <class T, class BodyType>
    class basic_stats_reply
        : public basic_stats_t<v10_detail::ofp_stats_reply, T, BodyType>
    {
    protected:
        using base_type
            = basic_stats_t<v10_detail::ofp_stats_reply, T, BodyType>;

    public:
        using base_type::base_type;
    };

} // namespace stats_detail
} // namespace statistics
} // namespace messages
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_BAISC_STATS_HPP
