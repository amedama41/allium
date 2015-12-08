#ifndef CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/bucket.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    namespace group_mod_detail {

        template <class T>
        class basic_group_mod
            : public v13_detail::basic_openflow_message<T>
        {
        public:
            static protocol::ofp_type const message_type
                = protocol::OFPT_GROUP_MOD;

        protected:
            basic_group_mod(
                      std::uint32_t const group_id
                    , protocol::ofp_group_type const group_type
                    , std::vector<bucket> buckets)
                : group_mod_{
                      v13_detail::ofp_header{
                          protocol::OFP_VERSION, message_type
                        , calc_length(buckets), basic_group_mod::get_xid()
                      }
                    , T::command_type, std::uint8_t(group_type), 0, group_id
                  }
                , buckets_(std::move(buckets))
            {
            }

            basic_group_mod(v13_detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
                : group_mod_(group_mod)
                , buckets_(std::move(buckets))
            {
            }

        public:
            auto header() const
                -> v13_detail::ofp_header const&
            {
                return group_mod_.header;
            }

            auto command() const
                -> protocol::ofp_group_mod_command
            {
                return T::command_type;
            }

            auto group_id() const
                -> std::uint32_t
            {
                return group_mod_.group_id;
            }

            auto group_type() const
                -> protocol::ofp_group_type
            {
                return protocol::ofp_group_type(group_mod_.type);
            }

            auto buckets() const
                -> std::vector<bucket> const&
            {
                return buckets_;
            }

            using v13_detail::basic_openflow_message<T>::encode;

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, group_mod_);
                boost::for_each(buckets_, [&](bucket const& bkt) {
                    bkt.encode(container);
                });
                return container;
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> T
            {
                auto const group_mod = detail::decode<v13_detail::ofp_group_mod>(first, last);
                if (std::distance(first, last) != group_mod.header.length - sizeof(v13_detail::ofp_group_mod)) {
                    throw 2;
                }
                auto buckets = std::vector<bucket>();
                buckets.reserve(std::distance(first, last) / (sizeof(v13_detail::ofp_bucket) + sizeof(v13_detail::ofp_action_header)));
                while (first != last) {
                    buckets.push_back(bucket::decode(first, last));
                }
                return T{group_mod, std::move(buckets)};
            }

        private:
            static auto calc_length(std::vector<bucket> const& buckets)
                -> std::uint16_t
            {
                using boost::adaptors::transformed;
                return boost::accumulate(
                          buckets | transformed([](bucket const& bkt) { return bkt.length(); })
                        , std::uint16_t(sizeof(v13_detail::ofp_group_mod)));
            }

        private:
            v13_detail::ofp_group_mod group_mod_;
            std::vector<bucket> buckets_;
        };

    } // namespace group_mod_detail

    class group_mod_add
        : public group_mod_detail::basic_group_mod<group_mod_add>
    {
    public:
        static protocol::ofp_group_mod_command const command_type
            = protocol::OFPGC_ADD;

        group_mod_add(
                  std::uint32_t const group_id
                , protocol::ofp_group_type const group_type
                , std::vector<bucket> buckets)
            : basic_group_mod{group_id, group_type, std::move(buckets)}
        {
        }

    private:
        friend basic_group_mod;

        group_mod_add(v13_detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
            : basic_group_mod{group_mod, std::move(buckets)}
        {
        }
    };

    class group_mod_modify
        : public group_mod_detail::basic_group_mod<group_mod_modify>
    {
    public:
        static protocol::ofp_group_mod_command const command_type
            = protocol::OFPGC_MODIFY;

        group_mod_modify(
                  std::uint32_t const group_id
                , protocol::ofp_group_type const group_type
                , std::vector<bucket> buckets)
            : basic_group_mod{group_id, group_type, std::move(buckets)}
        {
        }

    private:
        friend basic_group_mod;

        group_mod_modify(v13_detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
            : basic_group_mod{group_mod, std::move(buckets)}
        {
        }
    };

    class group_mod_delete
        : public v13_detail::basic_openflow_message<group_mod_delete>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_GROUP_MOD;
        static protocol::ofp_group_mod_command const command_type
            = protocol::OFPGC_DELETE;

        explicit group_mod_delete(std::uint32_t const group_id)
            : group_mod_{
                  v13_detail::ofp_header{
                      protocol::OFP_VERSION, message_type
                    , sizeof(v13_detail::ofp_group_mod), get_xid()
                  }
                , command_type, 0, 0, group_id
              }
        {
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return group_mod_.header;
        }

        auto command() const
            -> protocol::ofp_group_mod_command
        {
            return command_type;
        }

        auto group_id() const
            -> std::uint32_t
        {
            return group_mod_.group_id;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(container, group_mod_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> group_mod_delete
        {
            auto const group_mod = detail::decode<v13_detail::ofp_group_mod>(first, last);
            return group_mod_delete{group_mod};
        }

    private:
        explicit group_mod_delete(v13_detail::ofp_group_mod const& group_mod)
            : group_mod_(group_mod)
        {
        }

    private:
        v13_detail::ofp_group_mod group_mod_;
    };

} // namespace messages

using messages::group_mod_add;
using messages::group_mod_modify;
using messages::group_mod_delete;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP
