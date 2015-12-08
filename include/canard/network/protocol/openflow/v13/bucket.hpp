#ifndef CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/action_set.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class bucket
    {
    public:
        explicit bucket(action_set actions)
            : bucket{
                0, protocol::OFPP_ANY, protocol::OFPG_ANY, std::move(actions)
              }
        {
        }

        bucket(std::uint16_t const weight, action_set actions)
            : bucket{
                  weight, protocol::OFPP_ANY, protocol::OFPG_ANY
                , std::move(actions)
              }
        {
        }

        bucket(std::uint32_t const watch_port, std::uint32_t const watch_group, action_set actions)
            : bucket{0, watch_port, watch_group, std::move(actions)}
        {
        }

        bucket(std::uint16_t const weight, std::uint32_t const watch_port, std::uint32_t const watch_group
                , action_set actions)
            : bucket_{
                  std::uint16_t(sizeof(bucket_) + actions.length())
                , weight
                , watch_port, watch_group
                , {0, 0, 0, 0}
              }
            , actions_(std::move(actions))
        {
        }

        auto length() const
            -> std::uint16_t
        {
            return bucket_.len;
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
            auto const bkt = detail::decode<v13_detail::ofp_bucket>(first, last);
            if (std::distance(first, last) < bkt.len - sizeof(v13_detail::ofp_bucket)) {
                throw 2;
            }
            auto actions = action_set::decode(first, std::next(first, bkt.len - sizeof(v13_detail::ofp_bucket)));
            return bucket{bkt, std::move(actions)};
        }

    private:
        bucket(v13_detail::ofp_bucket const& bkt, action_set actions)
            : bucket_(bkt)
            , actions_(std::move(actions))
        {
        }

    private:
        v13_detail::ofp_bucket bucket_;
        action_set actions_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_BUCKET_HPP
