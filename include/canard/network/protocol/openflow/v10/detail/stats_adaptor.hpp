#ifndef CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATS_ADAPTOR_HPP
#define CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATS_ADAPTOR_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v10/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v10/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {
namespace v10_detail {

    namespace stats_adaptor_detail {

        template <
              class BodyType
            , typename std::enable_if<!std::is_same<BodyType, void>::value>::type* = nullptr
        >
        inline auto length()
            -> std::uint16_t
        {
            return sizeof(v10_detail::ofp_stats_request) + sizeof(BodyType);
        }

        template <
              class BodyType
            , typename std::enable_if<std::is_same<BodyType, void>::value>::type* = nullptr
        >
        inline auto length()
            -> std::uint16_t
        {
            return sizeof(v10_detail::ofp_stats_request);
        }

        template <class BodyType>
        inline auto num_of_body(std::uint16_t const length)
            -> std::size_t
        {
            return (length - sizeof(v10_detail::ofp_stats_reply))
                / BodyType::base_size;
        }

    } // namespace stats_adaptor_detail


    template <class T, class BodyType>
    class stats_request_adaptor
        : public v10_detail::basic_openflow_message<T>
    {
    public:
        static ofp_type const message_type = OFPT_STATS_REQUEST;

    protected:
        explicit stats_request_adaptor(std::uint32_t const xid)
            : stats_request_{
                  {
                      OFP_VERSION
                    , message_type
                    , stats_adaptor_detail::length<BodyType>()
                    , xid
                  }
                , T::stats_type_value
                , 0
              }
        {
        }

    public:
        auto header() const
            -> v10_detail::ofp_header
        {
            return stats_request_.header;
        }

        auto stats_type() const
            -> ofp_stats_types
        {
            return T::stats_type_value;
        }

        auto flags() const
            -> std::uint16_t
        {
            return stats_request_.flags;
        }

        auto more() const
            -> bool
        {
            return flags() & OFPSF_REPLY_MORE;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return encode_impl(
                    container, typename std::is_same<BodyType, void>::type{});
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            return decode_impl(
                    first, last, typename std::is_same<BodyType, void>::type{});
        }

    private:
        template <class Container>
        auto encode_impl(Container& container, std::true_type) const
            -> Container&
        {
            return detail::encode(container, stats_request_);
        }

        template <class Container>
        auto encode_impl(Container& container, std::false_type) const
            -> Container&
        {
            detail::encode(container, stats_request_);
            return detail::encode(
                    container, static_cast<T const*>(this)->body());
        }

        template <class Iterator>
        static auto decode_impl(Iterator& first, Iterator last, std::true_type)
            -> T
        {
            auto const stats_request
                = detail::decode<v10_detail::ofp_stats_request>(first, last);
            return T{stats_request};
        }

        template <class Iterator>
        static auto decode_impl(Iterator& first, Iterator last, std::false_type)
            -> T
        {
            auto const stats_request
                = detail::decode<v10_detail::ofp_stats_request>(first, last);
            auto const body = detail::decode<BodyType>(first, last);
            return T{stats_request, body};
        }

    protected:
        explicit stats_request_adaptor(
                  v10_detail::ofp_stats_request const& stats_request)
            : stats_request_(stats_request)
        {
            if (stats_request_.header.version != v10::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
        }

    private:
        v10_detail::ofp_stats_request stats_request_;
    };


    template <class T, class BodyType, bool IsArray>
    class stats_reply_adaptor
        : public v10_detail::basic_openflow_message<T>
    {
        using IsArrayType = typename std::conditional<
            IsArray, std::true_type, std::false_type
        >::type;

    public:
        static ofp_type const message_type = OFPT_STATS_REPLY;

    protected:
        stats_reply_adaptor(
                  std::uint32_t const xid
                , std::uint16_t const body_length, std::uint16_t const flags)
            : stats_reply_{
                  {
                      OFP_VERSION
                    , message_type
                    , std::uint16_t(sizeof(stats_reply_) + body_length)
                    , xid
                  }
                , T::stats_type_value
                , flags
              }
        {
        }

    public:
        auto header() const
            -> v10_detail::ofp_header
        {
            return stats_reply_.header;
        }

        auto stats_type() const
            -> ofp_stats_types
        {
            return T::stats_type_value;
        }

        auto flags() const
            -> std::uint16_t
        {
            return stats_reply_.flags;
        }

        auto more() const
            -> bool
        {
            return flags() & OFPSF_REPLY_MORE;
        }

        void more(bool const has_more)
        {
            if (has_more) {
                stats_reply_.flags |= OFPSF_REPLY_MORE;
            }
            else {
                stats_reply_.flags &= ~OFPSF_REPLY_MORE;
            }
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return encode_impl(container, IsArrayType{});
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            return decode_impl(first, last, IsArrayType{});
        }

    private:
        template <class Container>
        auto encode_impl(Container& container, std::true_type) const
            -> Container&
        {
            detail::encode(container, stats_reply_);
            boost::for_each(
                      static_cast<T const*>(this)->body()
                    , [&](BodyType const& body) { body.encode(container); });
            return container;
        }

        template <class Container>
        auto encode_impl(Container& container, std::false_type) const
            -> Container&
        {
            detail::encode(container, stats_reply_);
            return detail::encode(
                    container, static_cast<T const*>(this)->body());
        }

        template <class Iterator>
        static auto decode_impl(Iterator& first, Iterator last, std::true_type)
            -> T
        {
            auto const stats_reply
                = detail::decode<v10_detail::ofp_stats_reply>(first, last);
            auto body = std::vector<BodyType>{};
            body.reserve(
                    stats_adaptor_detail::num_of_body<BodyType>(stats_reply.header.length));
            while (first != last) {
                body.emplace_back(BodyType::decode(first, last));
            }
            return T{stats_reply, std::move(body)};
        }

        template <class Iterator>
        static auto decode_impl(Iterator& first, Iterator last, std::false_type)
            -> T
        {
            auto const stats_reply
                = detail::decode<v10_detail::ofp_stats_reply>(first, last);
            auto const body = detail::decode<BodyType>(first, last);
            return T{stats_reply, body};
        }

    protected:
        explicit stats_reply_adaptor(
                  v10_detail::ofp_stats_reply const& stats_reply)
            : stats_reply_(stats_reply)
        {
            if (stats_reply_.header.version != v10::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
        }

    private:
        v10_detail::ofp_stats_reply stats_reply_;
    };

} // namespace v10_detail
} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_MESSAGES_STATS_ADAPTOR_HPP
