#ifndef CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP
#define CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/v13/bucket.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class T>
        class basic_group_mod
            : public basic_openflow_message<T>
        {
        public:
            static ofp_type const message_type = OFPT_GROUP_MOD;

        protected:
            basic_group_mod(std::uint32_t const group_id, ofp_group_type const group_type, std::vector<bucket> buckets)
                : group_mod_{
                      {OFP_VERSION, message_type, calc_length(buckets), basic_group_mod::get_xid()}
                    , T::command_type, std::uint8_t(group_type), 0, group_id
                  }
                , buckets_(std::move(buckets))
            {
            }

            basic_group_mod(detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
                : group_mod_(group_mod)
                , buckets_(std::move(buckets))
            {
            }

        public:
            auto header() const
                -> detail::ofp_header const&
            {
                return group_mod_.header;
            }

            auto command() const
                -> ofp_group_mod_command
            {
                return T::command_type;
            }

            auto group_id() const
                -> std::uint32_t
            {
                return group_mod_.group_id;
            }

            auto group_type() const
                -> ofp_group_type
            {
                return ofp_group_type(group_mod_.type);
            }

            auto buckets() const
                -> std::vector<bucket> const&
            {
                return buckets_;
            }

            using basic_openflow_message<T>::encode;

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
                auto const group_mod = detail::decode<detail::ofp_group_mod>(first, last);
                if (std::distance(first, last) != group_mod.header.length - sizeof(detail::ofp_group_mod)) {
                    throw 2;
                }
                auto buckets = std::vector<bucket>();
                buckets.reserve(std::distance(first, last) / (sizeof(detail::ofp_bucket) + sizeof(detail::ofp_action_header)));
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
                        , std::uint16_t(sizeof(detail::ofp_group_mod)));
            }

        private:
            detail::ofp_group_mod group_mod_;
            std::vector<bucket> buckets_;
        };

    } // namespace detail

    class group_mod_add
        : public detail::basic_group_mod<group_mod_add>
    {
    public:
        static ofp_group_mod_command const command_type = OFPGC_ADD;

        group_mod_add(std::uint32_t const group_id, ofp_group_type const group_type, std::vector<bucket> buckets)
            : basic_group_mod{group_id, group_type, std::move(buckets)}
        {
        }

    private:
        friend basic_group_mod;

        group_mod_add(detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
            : basic_group_mod{group_mod, std::move(buckets)}
        {
        }
    };

    class group_mod_modify
        : public detail::basic_group_mod<group_mod_modify>
    {
    public:
        static ofp_group_mod_command const command_type = OFPGC_MODIFY;

        group_mod_modify(std::uint32_t const group_id, ofp_group_type const group_type, std::vector<bucket> buckets)
            : basic_group_mod{group_id, group_type, std::move(buckets)}
        {
        }

    private:
        friend basic_group_mod;

        group_mod_modify(detail::ofp_group_mod const& group_mod, std::vector<bucket> buckets)
            : basic_group_mod{group_mod, std::move(buckets)}
        {
        }
    };

    class group_mod_delete
        : public detail::basic_openflow_message<group_mod_delete>
    {
    public:
        static ofp_type const message_type = OFPT_GROUP_MOD;
        static ofp_group_mod_command const command_type = OFPGC_DELETE;

        explicit group_mod_delete(std::uint32_t const group_id)
            : group_mod_{
                  {OFP_VERSION, message_type, sizeof(detail::ofp_group_mod), get_xid()}
                , command_type, 0, 0, group_id
              }
        {
        }

        auto header() const
            -> detail::ofp_header const&
        {
            return group_mod_.header;
        }

        auto command() const
            -> ofp_group_mod_command
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
            auto const group_mod = detail::decode<detail::ofp_group_mod>(first, last);
            return group_mod_delete{group_mod};
        }

    private:
        explicit group_mod_delete(detail::ofp_group_mod const& group_mod)
            : group_mod_(group_mod)
        {
        }

    private:
        detail::ofp_group_mod group_mod_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_GROUP_MOD_HPP
