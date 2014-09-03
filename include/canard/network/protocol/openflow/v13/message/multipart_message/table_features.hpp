#ifndef CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURES_HPP
#define CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURES_HPP

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <map>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/visitors.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/basic_multipart.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/table_feature_property.hpp>
#include <canard/network/protocol/openflow/v13/utility/table_feature_property_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class table_features
    {
    public:
        using value_type = table_feature_property_set::value_type;
        using reference = table_feature_property_set::reference;
        using const_reference = table_feature_property_set::const_reference;
        using iterator = table_feature_property_set::const_iterator;
        using const_iterator = table_feature_property_set::const_iterator;

        table_features(std::uint8_t const table_id, std::string const& name
                , std::uint64_t const metadata_match, std::uint64_t const metadata_write
                , std::uint32_t const config, std::uint32_t const max_entries
                , table_feature_property_set properties)
            : table_features_{
                  std::uint16_t(sizeof(detail::ofp_table_features) + properties.length())
                , table_id, {0, 0, 0, 0, 0}, ""
                , metadata_match, metadata_write, config, max_entries
              }
            , properties_(std::move(properties))
        {
            std::strncpy(table_features_.name, name.c_str(), sizeof(table_features_.name));
        }

        auto length() const
            -> std::uint16_t
        {
            return table_features_.length;
        }

        auto table_id() const
            -> std::uint8_t
        {
            return table_features_.table_id;
        }

        auto name() const
            -> char const*
        {
            return table_features_.name;
        }

        auto metadata_match() const
            -> std::uint64_t
        {
            return table_features_.metadata_match;
        }

        auto metadata_write() const
            -> std::uint64_t
        {
            return table_features_.metadata_write;
        }

        auto config() const
            -> std::uint32_t
        {
            return table_features_.config;
        }

        auto max_entries() const
            -> std::uint32_t
        {
            return table_features_.max_entries;
        }

        auto begin() const
            -> const_iterator
        {
            return properties_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return properties_.end();
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
            auto const features = detail::decode<detail::ofp_table_features>(first, last);
            if (std::distance(first, last) < features.length - sizeof(detail::ofp_table_features)) {
                throw 2;
            }
            last = std::next(first, features.length - sizeof(detail::ofp_table_features));
            auto properties = table_feature_property_set::decode(first, last);
            return table_features{features, std::move(properties)};
        }

    private:
        table_features(detail::ofp_table_features const& features, table_feature_property_set properties)
            : table_features_(features)
            , properties_(std::move(properties))
        {
        }

    private:
        detail::ofp_table_features table_features_;
        table_feature_property_set properties_;
    };


    class table_features_request
        : public detail::basic_multipart_request<table_features_request>
    {
        using table_features_list = std::vector<table_features>;

    public:
        using const_iterator = table_features_list::const_iterator;

        static ofp_multipart_type const multipart_type_value = OFPMP_TABLE_FEATURES;

        table_features_request()
            : basic_multipart_request{0, 0}
            , features_{}
        {
        }

        table_features_request(std::initializer_list<table_features> features)
            : basic_multipart_request{
                  boost::accumulate(features | boost::adaptors::transformed([](table_features const& features) {
                        return features.length();
                  }), std::size_t{0})
                , 0
              }
            , features_{std::move(features)}
        {
        }

        auto begin() const
            -> const_iterator
        {
            return features_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return features_.end();
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            basic_multipart_request::encode(container);
            boost::for_each(features_, [&](table_features const& features) {
                features.encode(container);
            });
            return container;
        }

    private:
        table_features_list features_;
    };


    class table_features_reply
        : public detail::basic_multipart_reply<table_features_reply>
    {
        using table_features_list = std::vector<table_features>;

    public:
        using const_iterator = table_features_list::const_iterator;

        static ofp_multipart_type const multipart_type_value = OFPMP_TABLE_FEATURES;

        table_features_reply(std::initializer_list<table_features> features)
            : basic_multipart_reply{
                  boost::accumulate(features | boost::adaptors::transformed([](table_features const& features) {
                        return features.length();
                  }), std::size_t{0})
                , 0
              }
            , features_{std::move(features)}
        {
        }

        auto begin() const
            -> const_iterator
        {
            return features_.begin();
        }

        auto end() const
            -> const_iterator
        {
            return features_.end();
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> table_features_reply
        {
            auto const reply = basic_multipart_reply::decode(first, last);
            if (std::distance(first, last) != reply.header.length - sizeof(detail::ofp_multipart_reply)) {
                throw 2;
            }
            auto features = table_features_list{};
            features.reserve(std::distance(first, last) / (sizeof(detail::ofp_table_features) * 4));
            while (first != last) {
                features.emplace_back(table_features::decode(first, last));
            }
            return table_features_reply{reply, std::move(features)};
        }

    private:
        table_features_reply(detail::ofp_multipart_reply const& reply, table_features_list features)
            : basic_multipart_reply{reply}
            , features_(std::move(features))
        {
        }

    private:
        table_features_list features_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_TABLE_FEATURES_HPP
