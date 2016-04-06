#ifndef CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <boost/operators.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class action_id
        : private boost::equality_comparable<action_id>
    {
        using raw_ofp_type = v13_detail::ofp_action_header;

    public:
        explicit action_id(std::uint16_t const type) noexcept
            : type_(type)
        {
        }

        auto type() const noexcept
            -> protocol::ofp_action_type
        {
            return protocol::ofp_action_type(type_);
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return offsetof(v13_detail::ofp_action_header, pad);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(
                      container
                    , raw_ofp_type{std::uint16_t(type()), length()}
                    , length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_id
        {
            auto const action_header
                = detail::decode<raw_ofp_type>(first, last, length());
            return action_id{action_header.type};
        }

        static void validate_action_header(
                v13_detail::ofp_action_header const& header)
        {
            if (header.type == protocol::OFPAT_EXPERIMENTER) {
                throw std::runtime_error{"invalid action type"};
            }
            if (header.len != offsetof(v13_detail::ofp_action_header, pad)) {
                throw std::runtime_error{"action id length must be 4"};
            }
        }

    private:
        std::uint16_t type_;
    };

    inline auto operator==(action_id const& lhs, action_id const& rhs) noexcept
        -> bool
    {
        return lhs.type() == rhs.type();
    }


    class action_experimenter_id
        : private boost::equality_comparable<action_experimenter_id>
    {
        using raw_ofp_type = v13_detail::ofp_action_experimenter_header;

    public:
        explicit action_experimenter_id(std::uint32_t const experimenter)
            : experimenter_(experimenter)
            , data_{}
        {
        }

        action_experimenter_id(
                  std::uint32_t const experimenter
                , std::vector<unsigned char> data)
            : experimenter_(experimenter)
            , data_(std::move(data))
        {
        }

        static constexpr auto type() noexcept
            -> protocol::ofp_action_type
        {
            return protocol::OFPAT_EXPERIMENTER;
        }

        auto length() const noexcept
            -> std::uint16_t
        {
            return sizeof(raw_ofp_type) + data_.size();
        }

        auto experimenter() const noexcept
            -> std::uint32_t
        {
            return experimenter_;
        }

        auto data() const noexcept
            -> std::vector<unsigned char> const&
        {
            return data_;
        }

        auto extract_data()
            -> std::vector<unsigned char>
        {
            return std::move(data_);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            auto const exp_header = raw_ofp_type{
                std::uint16_t(type()), length(), experimenter()
            };
            detail::encode(container, exp_header);
            return detail::encode_byte_array(
                    container, data_.data(), data_.size());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> action_experimenter_id
        {
            auto const exp_header = detail::decode<raw_ofp_type>(first, last);

            last = std::next(first, exp_header.len - sizeof(raw_ofp_type));
            auto data = std::vector<unsigned char>(first, last);
            first = last;

            return action_experimenter_id{
                exp_header.experimenter, std::move(data)
            };
        }

        static void validate_action_header(
                v13_detail::ofp_action_header const& header)
        {
            if (header.type != protocol::OFPAT_EXPERIMENTER) {
                throw std::runtime_error{"invalid action type"};
            }
            if (header.len < sizeof(raw_ofp_type)) {
                throw std::runtime_error{
                    "experimenter action id length is too small"
                };
            }
        }

    private:
        std::uint32_t experimenter_;
        std::vector<unsigned char> data_;
    };

    inline auto operator==(
              action_experimenter_id const& lhs
            , action_experimenter_id const& rhs) noexcept
        -> bool
    {
        return lhs.experimenter() == rhs.experimenter()
            && lhs.data() == rhs.data();
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ACTION_ID_HPP
