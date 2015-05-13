#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class instruction_id
    {
    public:
        explicit instruction_id(std::uint16_t const type)
            : type_{type}
        {
        }

        auto type() const
            -> ofp_instruction_type
        {
            return ofp_instruction_type(type_);
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(detail::ofp_instruction);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, type_);
            return detail::encode(container, length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_id
        {
            auto const type = detail::decode<std::uint16_t>(first, last);
            auto const length = detail::decode<std::uint16_t>(first, last);
            if (length != sizeof(detail::ofp_instruction)) {
                throw std::runtime_error{__func__};
            }
            return instruction_id{type};
        }

    private:
        std::uint16_t type_;
    };


    class instruction_experimenter_id
    {
    public:
        explicit instruction_experimenter_id(std::uint32_t const experimenter)
            : experimenter_{experimenter}
            , data_{}
        {
        }

        instruction_experimenter_id(std::uint32_t const experimenter, std::vector<unsigned char> data)
            : experimenter_{experimenter}
            , data_(std::move(data))
        {
        }

        auto type() const
            -> ofp_instruction_type
        {
            return OFPIT_EXPERIMENTER;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(detail::ofp_instruction_experimenter) + data_.size();
        }

        auto experimenter() const
            -> std::uint32_t
        {
            return experimenter_;
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, std::uint16_t(type()));
            detail::encode(container, length());
            detail::encode(container, experimenter());
            return boost::push_back(container, data_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_experimenter_id
        {
            auto const experimenter_header
                = detail::decode<detail::ofp_instruction_experimenter>(first, last);
            if (experimenter_header.len > sizeof(experimenter_header) + std::distance(first, last)) {
                throw std::runtime_error{__func__};
            }
            auto const data_last = std::next(first, experimenter_header.len - sizeof(experimenter_header));
            auto data = std::vector<unsigned char>(first, data_last);
            first = data_last;
            return instruction_experimenter_id{
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

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
