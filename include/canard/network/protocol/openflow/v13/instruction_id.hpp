#ifndef CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
#define CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP

#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/any_instruction_id.hpp>
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

        static constexpr auto length()
            -> std::uint16_t
        {
            return sizeof(std::uint32_t);
        }

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            return detail::encode(detail::encode(container, type_), length());
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> instruction_id
        {
            auto const type = detail::decode<std::uint16_t>(first, last);
            auto const length = detail::decode<std::uint16_t>(first, last);
            if (length != sizeof(std::uint32_t)) {
                throw 2;
            }
            return instruction_id{type};
        }

    private:
        std::uint16_t type_;
    };


    class experimenter_instruction_id
    {
    public:
        explicit experimenter_instruction_id(std::uint32_t const experimenter)
            : experimenter_{experimenter}
            , data_{}
        {
        }

        experimenter_instruction_id(std::uint32_t const experimenter, std::vector<std::uint8_t> data)
            : experimenter_{experimenter}
            , data_(std::move(data))
        {
        }

        static constexpr auto type()
            -> ofp_instruction_type
        {
            return OFPIT_EXPERIMENTER;
        }

        auto length() const
            -> std::uint16_t
        {
            return sizeof(std::uint32_t) * 2 + data_.size();
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
            detail::encode(detail::encode(container, std::uint16_t{type()}), length());
            return boost::push_back(detail::encode(container, experimenter()), data_);
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> experimenter_instruction_id
        {
            std::advance(first, sizeof(std::uint16_t));
            auto const length = detail::decode<std::uint16_t>(first, last);
            auto const experimenter = detail::decode<std::uint32_t>(first, last);
            auto data = std::vector<std::uint8_t>(first, std::next(first, length - sizeof(std::uint32_t) * 2));
            std::advance(first, data.size());
            return experimenter_instruction_id{experimenter, std::move(data)};
        }

    private:
        std::uint32_t experimenter_;
        std::vector<std::uint8_t> data_;
    };

    template <class Iterator>
    inline auto decode_instruction_id(Iterator& first, Iterator last)
        -> any_instruction_id
    {
        auto copy_first = first;
        auto const type = detail::decode<std::uint16_t>(copy_first, last);
        switch (type) {
        case OFPIT_EXPERIMENTER:
            std::advance(copy_first, sizeof(std::uint16_t));
            switch (detail::decode<std::uint32_t>(copy_first, last)) {
            default:
                return experimenter_instruction_id::decode(first, last);
            }
        default:
            return instruction_id::decode(first, last);
        }
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_INSTRUCTION_ID_HPP
