#ifndef CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/action_list.hpp>
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class bucket
        : private boost::equality_comparable<bucket>
    {
        using raw_ofp_type = v13_detail::ofp_bucket;

    public:
        bucket(std::uint16_t const weight
             , std::uint32_t const watch_port
             , std::uint32_t const watch_group
             , action_set actions)
            : bucket_{
                  std::uint16_t(sizeof(bucket_) + actions.length())
                , weight
                , watch_port
                , watch_group
                , { 0, 0, 0, 0 }
              }
            , actions_(std::move(actions).to_list())
        {
        }

        explicit bucket(action_set actions)
            : bucket{
                0, protocol::OFPP_ANY, protocol::OFPG_ANY, std::move(actions)
              }
        {
        }

        bucket(std::uint16_t const weight, action_set actions)
            : bucket{
                  weight
                , protocol::OFPP_ANY, protocol::OFPG_ANY
                , std::move(actions)
              }
        {
        }

        bucket(std::uint32_t const watch_port
             , std::uint32_t const watch_group
             , action_set actions)
            : bucket{0, watch_port, watch_group, std::move(actions)}
        {
        }

        bucket(bucket const&) = default;

        bucket(bucket&& other)
            : bucket_(other.bucket_)
            , actions_(std::move(other).actions_)
        {
            other.bucket_.len = sizeof(raw_ofp_type);
        }

        auto operator=(bucket const&)
            -> bucket& = default;

        auto operator=(bucket&& other)
            -> bucket&
        {
            auto tmp = std::move(other);
            std::swap(bucket_, tmp.bucket_);
            actions_.swap(tmp.actions_);
            return *this;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return bucket_.len;
        }

        auto weight() const noexcept
            -> std::uint16_t
        {
            return bucket_.weight;
        }

        auto watch_port() const noexcept
            -> std::uint32_t
        {
            return bucket_.watch_port;
        }

        auto watch_group() const noexcept
            -> std::uint32_t
        {
            return bucket_.watch_group;
        }

        auto actions() const noexcept
            -> action_list const&
        {
            return actions_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, bucket_);
            return actions_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> bucket
        {
            auto const bkt = detail::decode<raw_ofp_type>(first, last);
            if (bkt.len < sizeof(raw_ofp_type)) {
                throw std::runtime_error{"bucket length is too small"};
            }
            if (std::distance(first, last) < bkt.len - sizeof(raw_ofp_type)) {
                throw std::runtime_error{"bucket length is too big"};
            }

            last = std::next(first, bkt.len - sizeof(raw_ofp_type));

            auto actions = action_list::decode(first, last);

            return bucket{bkt, std::move(actions)};
        }

        static auto all(action_set actions)
            -> bucket
        {
            return bucket{std::move(actions)};
        }

        static auto select(std::uint16_t const weight, action_set actions)
            -> bucket
        {
            return bucket{weight, std::move(actions)};
        }

        static auto indirect(action_set actions)
            -> bucket
        {
            return bucket{std::move(actions)};
        }

        static auto failover(
                  std::uint32_t const watch_port
                , std::uint32_t const watch_group
                , action_set actions)
            -> bucket
        {
            return bucket{watch_port, watch_group, std::move(actions)};
        }

        template <class T>
        static auto validate(T&& t)
            -> typename std::enable_if<
                  canard::is_same_value_type<T, bucket>::value, T&&
               >::type
        {
            if (!action_set::is_action_set(t.actions())) {
                throw std::runtime_error{"duplicated action type"};
            }
            return std::forward<T>(t);
        }

        template <class... Args>
        static auto create(Args&&... args)
            -> bucket
        {
            return validate(bucket(std::forward<Args>(args)...));
        }

    private:
        bucket(raw_ofp_type const& bkt, action_list&& actions)
            : bucket_(bkt)
            , actions_(std::move(actions))
        {
        }

    private:
        raw_ofp_type bucket_;
        action_list actions_;
    };

    inline auto operator==(bucket const& lhs, bucket const& rhs)
        -> bool
    {
        return lhs.length() == rhs.length()
            && lhs.weight() == rhs.weight()
            && lhs.watch_port() == rhs.watch_port()
            && lhs.watch_group() == rhs.watch_group()
            && lhs.actions() == rhs.actions();
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP
