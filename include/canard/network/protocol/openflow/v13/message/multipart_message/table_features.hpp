#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_FEATURES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_FEATURES_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/utility/string_ref.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/table_feature_property/table_feature_property.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/network/protocol/openflow/v13/utility/table_feature_property_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {
namespace multipart {

    class table_features
    {
        using raw_ofp_type = v13_detail::ofp_table_features;

    public:
        static constexpr std::size_t base_size = sizeof(raw_ofp_type);

        table_features(
                  std::uint8_t const table_id
                , boost::string_ref const name
                , std::uint64_t const metadata_match
                , std::uint64_t const metadata_write
                , std::uint32_t const config
                , std::uint32_t const max_entries
                , table_feature_property_set properties)
            : table_features_{
                  std::uint16_t(
                            sizeof(v13_detail::ofp_table_features)
                          + properties.length())
                , table_id
                , { 0, 0, 0, 0, 0 }
                , ""
                , metadata_match
                , metadata_write
                , config
                , max_entries
              }
            , properties_(std::move(properties))
        {
            auto const name_size
                = std::min(name.size(), sizeof(table_features_.name) - 1);
            using boost::adaptors::sliced;
            boost::copy(name | sliced(0, name_size), table_features_.name);
        }

        table_features(table_features const&) = default;

        table_features(table_features&& other)
            : table_features_(other.table_features_)
            , properties_(std::move(other).properties_)
        {
            other.table_features_.length = base_size;
        }

        auto operator=(table_features const&)
            -> table_features& = default;

        auto operator=(table_features&& other)
            -> table_features&
        {
            auto tmp = std::move(other);
            std::swap(table_features_, tmp.table_features_);
            properties_.swap(properties_);
            return *this;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return table_features_.length;
        }

        auto table_id() const noexcept
            -> std::uint8_t
        {
            return table_features_.table_id;
        }

        auto name() const
            -> boost::string_ref
        {
            return table_features_.name;
        }

        auto metadata_match() const noexcept
            -> std::uint64_t
        {
            return table_features_.metadata_match;
        }

        auto metadata_write() const noexcept
            -> std::uint64_t
        {
            return table_features_.metadata_write;
        }

        auto config() const noexcept
            -> std::uint32_t
        {
            return table_features_.config;
        }

        auto max_entries() const noexcept
            -> std::uint32_t
        {
            return table_features_.max_entries;
        }

        auto properties() const noexcept
            -> table_feature_property_set const&
        {
            return properties_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, table_features_);
            return properties_.encode(container);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_features
        {
            auto const features = detail::decode<raw_ofp_type>(first, last);
            if (features.length < sizeof(raw_ofp_type)) {
                throw std::runtime_error{"table_features length is too small"};
            }
            if (std::distance(first, last)
                    < features.length - sizeof(raw_ofp_type)) {
                throw std::runtime_error{"table_features length is too big"};
            }

            last = std::next(first, features.length - sizeof(raw_ofp_type));
            auto properties = table_feature_property_set::decode(first, last);
            return table_features{features, std::move(properties)};
        }

    private:
        table_features(
                  v13_detail::ofp_table_features const& features
                , table_feature_property_set&& properties)
            : table_features_(features)
            , properties_(std::move(properties))
        {
        }

    private:
        v13_detail::ofp_table_features table_features_;
        table_feature_property_set properties_;
    };


    class table_features_request
        : public multipart_detail::basic_multipart_request<
                table_features_request, table_features[]
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_TABLE_FEATURES;

        explicit table_features_request(std::uint32_t const xid = get_xid())
            : basic_multipart_request{0, {}, xid}
        {
        }

        table_features_request(
                  body_type table_features
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_multipart_request{flags, std::move(table_features), xid}
        {
        }

    private:
        friend basic_multipart_request::base_type;

        table_features_request(
                  v13_detail::ofp_multipart_request const& multipart_request
                , body_type&& table_features)
            : basic_multipart_request{
                multipart_request, std::move(table_features)
              }
        {
        }
    };


    class table_features_reply
        : public multipart_detail::basic_multipart_reply<
                table_features_reply, table_features[]
          >
    {
    public:
        static constexpr protocol::ofp_multipart_type multipart_type_value
            = protocol::OFPMP_TABLE_FEATURES;

        table_features_reply(
                  body_type table_features
                , std::uint16_t const flags = 0
                , std::uint32_t const xid = get_xid())
            : basic_multipart_reply{flags, std::move(table_features), xid }
        {
        }

    private:
        friend basic_multipart_reply::base_type;

        table_features_reply(
                  v13_detail::ofp_multipart_reply const& multipart_reply
                , body_type&& table_features)
            : basic_multipart_reply{multipart_reply, std::move(table_features)}
        {
        }
    };

} // namespace multipart
} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_MULTIPART_TABLE_FEATURES_HPP
