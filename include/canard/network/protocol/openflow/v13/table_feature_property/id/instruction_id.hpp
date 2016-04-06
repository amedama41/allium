#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP

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

    class instruction_id
        : private boost::equality_comparable<instruction_id>
    {
        using raw_ofp_type = v13_detail::ofp_instruction;

    public:
        explicit instruction_id(std::uint16_t const type) noexcept
            : type_{type}
        {
        }

        auto type() const noexcept
            -> protocol::ofp_instruction_type
        {
            return protocol::ofp_instruction_type(type_);
        }

        static constexpr auto length() noexcept
            -> std::uint16_t
        {
            return sizeof(v13_detail::ofp_instruction);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(
                    container, raw_ofp_type{std::uint16_t(type()), length()});
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_id
        {
            auto const id = detail::decode<raw_ofp_type>(first, last);
            return instruction_id{id.type};
        }

        static void validate_instruction_header(
                v13_detail::ofp_instruction const& instruction)
        {
            if (instruction.type == protocol::OFPIT_EXPERIMENTER) {
                throw std::runtime_error{"invalid instruction type"};
            }
            if (instruction.len != length()) {
                throw std::runtime_error{"instruction id length must be 4"};
            }
        }

    private:
        std::uint16_t type_;
    };

    inline auto operator==(
            instruction_id const& lhs, instruction_id const& rhs) noexcept
        -> bool
    {
        return lhs.type() == rhs.type();
    }


    class instruction_experimenter_id
    {
        using raw_ofp_type = v13_detail::ofp_instruction_experimenter;

    public:
        explicit instruction_experimenter_id(
                std::uint32_t const experimenter)
            : experimenter_(experimenter)
            , data_{}
        {
        }

        instruction_experimenter_id(
                  std::uint32_t const experimenter
                , std::vector<unsigned char> data)
            : experimenter_(experimenter)
            , data_(std::move(data))
        {
        }

        static constexpr auto type() noexcept
            -> protocol::ofp_instruction_type
        {
            return protocol::OFPIT_EXPERIMENTER;
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
            -> instruction_experimenter_id
        {
            auto const exp_header = detail::decode<raw_ofp_type>(first, last);

            last = std::next(first, exp_header.len - sizeof(raw_ofp_type));
            auto data = std::vector<unsigned char>(first, last);
            first = last;
            return instruction_experimenter_id{
                exp_header.experimenter, std::move(data)
            };
        }

        static void validate_instruction_header(
                v13_detail::ofp_instruction const& instruction)
        {
            if (instruction.type != protocol::OFPIT_EXPERIMENTER) {
                throw std::runtime_error{"invalid instruction type"};
            }
            if (instruction.len
                    < sizeof(v13_detail::ofp_instruction_experimenter)) {
                throw std::runtime_error{
                    "instruction experimenter id length is too small"
                };
            }
        }

    private:
        std::uint32_t experimenter_;
        std::vector<unsigned char> data_;
    };

    inline auto operator==(
              instruction_experimenter_id const& lhs
            , instruction_experimenter_id const& rhs) noexcept
        -> bool
    {
        return lhs.experimenter() == rhs.experimenter()
            && lhs.data() == rhs.data();
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
