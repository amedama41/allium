#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_BASIC_MULTIPART_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_BASIC_MULTIPART_HPP

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
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/oxm_match_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {
namespace multipart_detail {

    template <class MultipartType>
    struct multipart_message_type;

    template <>
    struct multipart_message_type<v13_detail::ofp_multipart_request>
    {
        static constexpr protocol::ofp_type value
            = protocol::OFPT_MULTIPART_REQUEST;
    };

    template <>
    struct multipart_message_type<v13_detail::ofp_multipart_reply>
    {
        static constexpr protocol::ofp_type value
            = protocol::OFPT_MULTIPART_REPLY;
    };

    template <class T, class MultipartType>
    class basic_multipart
        : public v13_detail::basic_openflow_message<T>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = multipart_message_type<MultipartType>::value;

        auto header() const noexcept
            -> v13_detail::ofp_header const&
        {
            return static_cast<T const*>(this)->multipart().header;
        }

        auto multipart_type() const noexcept
            -> protocol::ofp_multipart_type
        {
            return T::multipart_type_value;
        }

        auto flags() const noexcept
            -> std::uint16_t
        {
            return static_cast<T const*>(this)->multipart().flags;
        }

        static void validate(v13_detail::ofp_header const& header)
        {
            if (header.version != protocol::OFP_VERSION) {
                throw std::runtime_error{"invalid version"};
            }
            if (header.type != message_type) {
                throw std::runtime_error{"invalid message type"};
            }
            if (header.length < sizeof(MultipartType)) {
                throw std::runtime_error{"invalid length"};
            }
        }
    };


    template <class T, class MultipartType>
    class empty_body_multipart
        : public basic_multipart<T, MultipartType>
    {
    public:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, multipart_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            return T{detail::decode<MultipartType>(first, last)};
        }

        using basic_multipart<T, MultipartType>::validate;

        static void validate(MultipartType const& multipart)
        {
            if (multipart.type != T::multipart_type_value) {
                throw std::runtime_error{"invalid multipart type"};
            }
            if (multipart.header.length != sizeof(MultipartType)) {
                throw std::runtime_error{"invalid multipart length"};
            }
        }

    protected:
        empty_body_multipart(
                std::uint16_t const flags, std::uint32_t const xid) noexcept
            : multipart_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , sizeof(MultipartType)
                    , xid
                  }
                , T::multipart_type_value
                , flags
                , { 0, 0, 0, 0 }
              }
        {
        }

        explicit empty_body_multipart(MultipartType const& multipart) noexcept
            : multipart_(multipart)
        {
        }

    private:
        friend basic_multipart<T, MultipartType>;

        auto multipart() const noexcept
            -> MultipartType const&
        {
            return multipart_;
        }

    private:
        MultipartType multipart_;
    };


    template <class T, class BodyType, class MultipartType>
    class single_element_multipart
        : public basic_multipart<T, MultipartType>
    {
    public:
        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, multipart_);
            return detail::encode(container, body_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const multipart = detail::decode<MultipartType>(first, last);
            return T{multipart, detail::decode<BodyType>(first, last)};
        }

        using basic_multipart<T, MultipartType>::validate;

        static void validate(MultipartType const& multipart)
        {
            if (multipart.type != T::multipart_type_value) {
                throw std::runtime_error{"invalid multipart type"};
            }
            if (multipart.header.length
                    != sizeof(MultipartType) + sizeof(BodyType)) {
                throw std::runtime_error{"invalid multipart length"};
            }
        }

    protected:
        single_element_multipart(
                  std::uint16_t const flags
                , BodyType const& body
                , std::uint32_t const xid) noexcept
            : multipart_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , sizeof(MultipartType) + sizeof(BodyType)
                    , xid
                  }
                , T::multipart_type_value
                , flags
                , { 0, 0, 0, 0 }
              }
            , body_(body)
        {
        }

        single_element_multipart(
                MultipartType const& multipart, BodyType const& body) noexcept
            : multipart_(multipart)
            , body_(body)
        {
        }

        auto body() const noexcept
            -> BodyType const&
        {
            return body_;
        }

    private:
        friend basic_multipart<T, MultipartType>;

        auto multipart() const noexcept
            -> MultipartType const&
        {
            return multipart_;
        }

    private:
        MultipartType multipart_;
        BodyType body_;
    };


    template <class T, class BodyType, class MultipartType>
    class single_element_with_match_multipart
        : public basic_multipart<T, MultipartType>
    {
        static constexpr std::size_t base_multipart_size
            = sizeof(MultipartType)
            + sizeof(BodyType) + sizeof(v13_detail::ofp_match);

    public:
        auto match() const noexcept
            -> oxm_match_set const&
        {
            return match_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, multipart_);
            detail::encode(container, body_);
            return match_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const multipart = detail::decode<MultipartType>(first, last);
            last = std::next(
                    first, multipart.header.length - sizeof(MultipartType));

            auto const body = detail::decode<BodyType>(first, last);

            auto it = first;
            auto const ofp_match
                = detail::decode<v13_detail::ofp_match>(it, last);
            oxm_match_set::validate_ofp_match(ofp_match);
            if (std::distance(first, last)
                    != v13_detail::exact_length(ofp_match.length)) {
                throw std::runtime_error{"invalid oxm_match length"};
            }
            auto match = oxm_match_set::decode(first, last);

            return T{multipart, body, std::move(match)};
        }

        using basic_multipart<T, MultipartType>::validate;

        static void validate(MultipartType const& multipart)
        {
            if (multipart.type != T::multipart_type_value) {
                throw std::runtime_error{"invalid multipart type"};
            }
            if (multipart.header.length < base_multipart_size) {
                throw std::runtime_error{"invalid multipart length"};
            }
        }

    protected:
        single_element_with_match_multipart(
                  std::uint16_t const flags
                , BodyType const& body
                , oxm_match_set&& match
                , std::uint32_t const xid) noexcept
            : multipart_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , std::uint16_t(
                              sizeof(MultipartType)
                            + sizeof(BodyType)
                            + v13_detail::exact_length(match.length()))
                    , xid
                  }
                , T::multipart_type_value
                , flags
                , { 0, 0, 0, 0 }
              }
            , body_(body)
            , match_(std::move(match))
        {
        }

        single_element_with_match_multipart(
                  MultipartType const& multipart
                , BodyType const& body
                , oxm_match_set&& match) noexcept
            : multipart_(multipart)
            , body_(body)
            , match_(std::move(match))
        {
        }

        single_element_with_match_multipart(
                single_element_with_match_multipart const&) = default;

        single_element_with_match_multipart(
                single_element_with_match_multipart&& other)
            : multipart_(other.multipart_)
            , body_(other.body_)
            , match_(std::move(other).match_)
        {
            other.multipart_.header.length = base_multipart_size;
        }

        auto operator=(single_element_with_match_multipart const&)
            -> single_element_with_match_multipart& = default;

        auto operator=(single_element_with_match_multipart&& other)
            -> single_element_with_match_multipart&
        {
            auto tmp = std::move(other);
            std::swap(multipart_, tmp.multipart_);
            std::swap(body_, tmp.body_);
            match_.swap(tmp.match_);
            return *this;
        }

        auto body() const noexcept
            -> BodyType const&
        {
            return body_;
        }

    private:
        friend basic_multipart<T, MultipartType>;

        auto multipart() const noexcept
            -> MultipartType const&
        {
            return multipart_;
        }

    private:
        MultipartType multipart_;
        BodyType body_;
        oxm_match_set match_;
    };


    template <class T, class BodyType, class MultipartType>
    class array_body_multipart
        : public basic_multipart<T, MultipartType>
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
            detail::encode(container, multipart_);
            boost::for_each(
                      body_
                    , [&](elem_type const& body) { body.encode(container); });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> T
        {
            auto const multipart = detail::decode<MultipartType>(first, last);
            auto const body_length
                = multipart.header.length - sizeof(MultipartType);
            last = std::next(first, body_length);

            auto body = body_type{};
            body.reserve(body_length / elem_type::base_size);
            while (std::distance(first, last) >= elem_type::base_size) {
                body.push_back(elem_type::decode(first, last));
            }

            if (first != last) {
                throw std::runtime_error{"invalid multipart length"};
            }
            return T{multipart, std::move(body)};
        }

        using basic_multipart<T, MultipartType>::validate;

        static void validate(MultipartType const& multipart)
        {
            if (multipart.type != T::multipart_type_value) {
                throw std::runtime_error{"invalid multipart type"};
            }
            if (multipart.header.length < sizeof(MultipartType)) {
                throw std::runtime_error{"invalid multipart length"};
            }
        }

    protected:
        array_body_multipart(
                  std::uint16_t const flags
                , body_type&& body
                , std::uint32_t const xid)
            : multipart_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION
                    , T::message_type
                    , std::uint16_t(
                            sizeof(MultipartType) + calc_body_length(body))
                    , xid
                  }
                , T::multipart_type_value
                , flags
                , { 0, 0, 0, 0 }
              }
            , body_(std::move(body))
        {
        }

        array_body_multipart(
                MultipartType const& multipart, body_type&& body)
            : multipart_(multipart)
            , body_(std::move(body))
        {
        }

        array_body_multipart(array_body_multipart const&)
            = default;

        array_body_multipart(array_body_multipart&& other)
            : multipart_(other.multipart_)
            , body_(std::move(other).body_)
        {
            other.multipart_.header.length = sizeof(MultipartType);
        }

        auto operator=(array_body_multipart const&)
            -> array_body_multipart& = default;

        auto operator=(array_body_multipart&& other)
            -> array_body_multipart&
        {
            auto tmp = std::move(other);
            std::swap(multipart_, tmp.multipart_);
            body_.swap(tmp.body_);
            return *this;
        }

    private:
        friend basic_multipart<T, MultipartType>;

        auto multipart() const noexcept
            -> MultipartType const&
        {
            return multipart_;
        }

        static auto calc_body_length(body_type const& body)
            -> std::uint16_t
        {
            auto const body_length = boost::accumulate(
                      body, std::size_t{0}
                    , [](std::size_t const sum, elem_type const& e) {
                            return sum + e.length();
                      });
            if (body_length + sizeof(MultipartType)
                    > std::numeric_limits<std::uint16_t>::max()) {
                throw std::runtime_error{"body size is too big"};
            }
            return std::uint16_t(body_length);
        }

    private:
        MultipartType multipart_;
        body_type body_;
    };


    template <class MultipartType, class T, class BodyType, bool HasMatch>
    using basic_multipart_t = typename std::conditional<
          std::is_same<BodyType, void>::value
        , empty_body_multipart<T, MultipartType>
        , typename std::conditional<
              std::is_array<BodyType>::value
            , array_body_multipart<T, BodyType, MultipartType>
            , typename std::conditional<
                  HasMatch
                , single_element_with_match_multipart<
                        T, BodyType, MultipartType
                  >
                , single_element_multipart<T, BodyType, MultipartType>
              >::type
          >::type
    >::type;

    template <class T, class BodyType, bool HasMatch = false>
    class basic_multipart_request
         : public basic_multipart_t<
                v13_detail::ofp_multipart_request, T, BodyType, HasMatch
           >
    {
    protected:
        using base_type = basic_multipart_t<
            v13_detail::ofp_multipart_request, T, BodyType, HasMatch
        >;

    public:
        using base_type::base_type;
    };


    template <class T, class BodyType, bool HasMatch = false>
    class basic_multipart_reply
         : public basic_multipart_t<
                v13_detail::ofp_multipart_reply, T, BodyType, HasMatch
           >
    {
    protected:
        using base_type = basic_multipart_t<
            v13_detail::ofp_multipart_reply, T, BodyType, HasMatch
        >;

    public:
        using base_type::base_type;
    };

} // namespace multipart_detail
} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_BASIC_MULTIPART_HPP
