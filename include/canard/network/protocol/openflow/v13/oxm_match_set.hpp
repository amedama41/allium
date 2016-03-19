#ifndef CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_SET_HPP
#define CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_SET_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/numeric.hpp>
#include <canard/mpl/adapted/std_tuple.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/detail/padding.hpp>
#include <canard/network/protocol/openflow/v13/any_oxm_match_field.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>
#include <canard/type_traits.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class oxm_match_set
        : private boost::equality_comparable<oxm_match_set>
    {
        using container_type = std::map<std::uint32_t, any_oxm_match_field>;

    public:
        static constexpr protocol::ofp_match_type match_type
            = protocol::OFPMT_OXM;
        static constexpr std::size_t base_length
            = offsetof(v13_detail::ofp_match, pad);

        using key_type = std::uint32_t;
        using value_type = any_oxm_match_field;
        using reference = value_type const&;
        using const_reference = value_type const&;
        using iterator = boost::range_iterator<
            boost::select_second_const_range<container_type>
        >::type;
        using const_iterator = iterator;
        using difference_type = container_type::difference_type;
        using size_type = container_type::size_type;

        template <
              class... OXMMatchFields
            , typename std::enable_if<
                    !is_related<oxm_match_set, OXMMatchFields...>::value
              >::type* = nullptr
        >
        oxm_match_set(OXMMatchFields&&... oxm_fields)
            : oxm_tlvs_{to_pair(std::forward<OXMMatchFields>(oxm_fields))...}
        {
        }

        constexpr auto type() const noexcept
            -> protocol::ofp_match_type
        {
            return match_type;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            using boost::adaptors::transformed;
            using pair = container_type::value_type;
            return boost::accumulate(
                      oxm_tlvs_ | transformed(
                          [](pair const& p) { return p.second.length(); })
                    , std::uint16_t{base_length});
        }

        auto begin() const noexcept
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.begin()};
        }

        auto end() const noexcept
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.end()};
        }

        auto cbegin() const noexcept
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.cbegin()};
        }

        auto cend() const noexcept
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.cend()};
        }

        auto empty() const noexcept
            -> bool
        {
            return oxm_tlvs_.empty();
        }

        auto size() const noexcept
            -> size_type
        {
            return oxm_tlvs_.size();
        }

        auto max_size() const noexcept
            -> size_type
        {
            return oxm_tlvs_.max_size();
        }

        template <class OXMMatchField>
        auto get() const
            -> OXMMatchField const&
        {
            auto const it = oxm_tlvs_.find(OXMMatchField::oxm_type());
            return any_cast<OXMMatchField>(it->second);
        }

        auto at(key_type const oxm_type) const
            -> any_oxm_match_field const&
        {
            return oxm_tlvs_.at(oxm_type);
        }

        template <class OXMMatchField>
        auto insert(OXMMatchField const& field)
            -> std::pair<const_iterator, bool>
        {
            auto const result = oxm_tlvs_.emplace(field.oxm_type(), field);
            return make_result(result.first, result.second);
        }

        template <class OXMMatchField>
        auto assign(OXMMatchField const& field)
            -> std::pair<const_iterator, bool>
        {
            auto const it = oxm_tlvs_.find(field.oxm_type());
            if (it == oxm_tlvs_.end()) {
                return make_result(oxm_tlvs_.end(), false);
            }
            it->second = field;
            return make_result(it, true);
        }

        template <class OXMMatchField>
        auto insert_or_assign(OXMMatchField const& field)
            -> std::pair<const_iterator, bool>
        {
            auto const oxm_type = field.oxm_type();
            auto const it = oxm_tlvs_.lower_bound(oxm_type);
            if (it != oxm_tlvs_.end()
                    && !oxm_tlvs_.key_comp()(oxm_type, it->first)) {
                it->second = field;
                return make_result(it, false);
            }
            else {
                auto const result = oxm_tlvs_.emplace_hint(it, oxm_type, field);
                return make_result(result, true);
            }
        }

        template <class OXMMatchField>
        auto erase()
            -> size_type
        {
            return oxm_tlvs_.erase(OXMMatchField::oxm_type());
        }

        auto erase(const_iterator it)
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.erase(it.base())};
        }

        void swap(oxm_match_set& other)
        {
            oxm_tlvs_.swap(other.oxm_tlvs_);
        }

        void clear() noexcept
        {
            oxm_tlvs_.clear();
        }

        auto find(key_type const oxm_type) const
            -> const_iterator
        {
            return const_iterator{oxm_tlvs_.find(oxm_type)};
        }

        template <class OXMMatchField>
        auto find() const
            -> boost::optional<OXMMatchField const&>
        {
            auto const it = oxm_tlvs_.find(OXMMatchField::oxm_type());
            if (it == oxm_tlvs_.end()) {
                return boost::none;
            }
            return v13::any_cast<OXMMatchField const&>(it->second);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t{match_type});
            detail::encode(container, length());
            boost::for_each(*this, [&](any_oxm_match_field const& field) {
                field.encode(container);
            });
            return detail::encode_byte_array(
                      container
                    , detail::padding
                    , v13_detail::padding_length(length()));
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> oxm_match_set
        {
            std::advance(first, sizeof(std::uint16_t));
            auto const length = detail::decode<std::uint16_t>(first, last);
            last = std::next(first, length - base_length);

            auto oxm_tlvs = container_type{};
            using oxm_header = std::uint32_t;
            while (std::distance(first, last) >= sizeof(oxm_header)) {
                oxm_match_field_decoder::decode<void>(
                        first, last, oxm_tlv_inserter{oxm_tlvs});
            }
            if (first != last) {
                throw std::runtime_error{"invalid oxm_match length"};
            }

            std::advance(first, v13_detail::padding_length(length));

            return oxm_match_set{std::move(oxm_tlvs)};
        }

        static void validate_ofp_match(v13_detail::ofp_match const& match)
        {
            if (match.type != protocol::OFPMT_OXM) {
                throw std::runtime_error{"match_type is not OFPMT_OXM"};
            }
            if (match.length < base_length) {
                throw std::runtime_error{"oxm_match length is too short"};
            }
        }

    private:
        oxm_match_set(container_type&& oxm_tlvs)
            : oxm_tlvs_(std::move(oxm_tlvs))
        {
        }

        template <class OXMMatchField>
        static auto to_pair(OXMMatchField&& oxm_field)
            -> container_type::value_type
        {
            auto const oxm_type = oxm_field.oxm_type();
            return container_type::value_type{
                oxm_type, std::forward<OXMMatchField>(oxm_field)
            };
        }

        static auto make_result(container_type::iterator it, bool const result)
            -> std::pair<const_iterator, bool>
        {
            return std::make_pair(const_iterator{it}, result);
        }

        struct oxm_tlv_inserter
        {
            template <class OXMMatchField>
            void operator()(OXMMatchField&& field) const
            {
                if (!oxm_tlvs.emplace(field.oxm_type(), field).second) {
                    throw std::runtime_error{"duplicated oxm field type"};
                }
            }

            container_type& oxm_tlvs;
        };

    private:
        container_type oxm_tlvs_;
    };

    namespace oxm_match_set_detail {

        struct oxm_match_set_equal_check
        {
            template <class OXMMatchField>
            auto operator()(bool const result, OXMMatchField*) const
                -> bool
            {
                auto const lhs_field = lhs.find<OXMMatchField>();
                auto const rhs_field = rhs.find<OXMMatchField>();
                if (lhs_field == rhs_field
                        || (!lhs_field && rhs_field->is_wildcard())
                        || (!rhs_field && lhs_field->is_wildcard())) {
                    return result;
                }
                return false;
            }

            oxm_match_set const& lhs;
            oxm_match_set const& rhs;
        };

    } // namespace oxm_match_set_detail

    inline auto operator==(oxm_match_set const& lhs, oxm_match_set const& rhs)
        -> bool
    {
        using match_field_pointer_list = boost::mpl::transform<
            any_oxm_match_field::type_list, std::add_pointer<boost::mpl::_>
        >::type;
        return boost::fusion::accumulate(
                  match_field_pointer_list{}
                , true
                , oxm_match_set_detail::oxm_match_set_equal_check{lhs, rhs});
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_OXM_MATCH_SET_HPP
