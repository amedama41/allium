#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/format.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_id
    {
    public:
        explicit action_id(std::uint16_t const type)
            : type_(type)
        {
        }

        auto type() const
            -> protocol::ofp_action_type
        {
            return protocol::ofp_action_type(type_);
        }

        auto length() const
            -> std::uint16_t
        {
            return offsetof(v13_detail::ofp_action_header, pad);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t(type()));
            return detail::encode(container, length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_id
        {
            auto const type = detail::decode<std::uint16_t>(first, last);
            auto const length = detail::decode<std::uint16_t>(first, last);
            if (length != offsetof(v13_detail::ofp_action_header, pad)) {
                throw std::runtime_error{
                    boost::str(boost::format{
                        "%s: ofp_action_header:length is invalid, expected %u but %u"
                    } % __func__ % offsetof(v13_detail::ofp_action_header, pad) % length)
                };
            }
            return action_id{type};
        }

    private:
        std::uint16_t type_;
    };

    class action_experimenter_id
    {
    public:
        action_experimenter_id(std::uint32_t const experimenter, std::vector<unsigned char> data)
            : experimenter_(experimenter)
            , data_(std::move(data))
        {
        }

        auto type() const
            -> protocol::ofp_action_type
        {
            return protocol::OFPAT_EXPERIMENTER;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(v13_detail::ofp_action_experimenter_header) + data_.size();
        }

        auto experimenter() const
            -> std::uint32_t
        {
            return experimenter_;
        }

        auto data() const
            -> std::vector<unsigned char> const&
        {
            return data_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t(type()));
            detail::encode(container, length());
            detail::encode(container, experimenter());
            return boost::push_back(container, data());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_experimenter_id
        {
            auto const experimenter_header
                = detail::decode<v13_detail::ofp_action_experimenter_header>(first, last);
            if (experimenter_header.len > sizeof(experimenter_header) + std::distance(first, last)) {
                throw std::runtime_error{__func__};
            }
            auto const data_last
                = std::next(first, experimenter_header.len - sizeof(experimenter_header));
            auto data = std::vector<unsigned char>(first, data_last);
            first = data_last;
            return action_experimenter_id{
                experimenter_header.experimenter, std::move(data)
            };
        }

    private:
        std::uint32_t experimenter_;
        std::vector<unsigned char> data_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
