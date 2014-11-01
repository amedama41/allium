#ifndef CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP
#define CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP

#include <cstdint>
#include <boost/variant/static_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    namespace detail {

        template <class Container>
        class encoding_visitor
            : public boost::static_visitor<Container&>
        {
        public:
            encoding_visitor(Container& container)
                : container_{&container}
            {
            }

            template <class T>
            auto operator()(T const& t) const
                -> Container&
            {
                return t.encode(*container_);
            }

        private:
            Container* container_;
        };

        template <class Type>
        class type_visitor
            : public boost::static_visitor<Type>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> Type
            {
                return t.type();
            }
        };

        class length_visitor
            : public boost::static_visitor<std::uint16_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint16_t
            {
                return t.length();
            }
        };

        class calculating_exact_length_visitor
            : public boost::static_visitor<std::uint16_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint16_t
            {
                return detail::exact_length(t.length());
            }
        };

        class instruction_order_visitor
            : public boost::static_visitor<std::uint64_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint64_t
            {
                return instruction_order(t);
            }
        };

        class oxm_type_visitor
            : public boost::static_visitor<std::uint32_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint32_t
            {
                return t.oxm_type();
            }
        };

        class oxm_header_visitor
            : public boost::static_visitor<std::uint32_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint32_t
            {
                return t.oxm_header();
            }
        };

        class oxm_has_mask_visitor
            : public boost::static_visitor<bool>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> bool
            {
                return t.oxm_has_mask();
            }
        };

        class oxm_length_visitor
            : public boost::static_visitor<std::uint8_t>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> std::uint8_t
            {
                return t.oxm_length();
            }
        };

        class wildcard_visitor
            : public boost::static_visitor<bool>
        {
        public:
            template <class T>
            auto operator()(T const& t) const
                -> bool
            {
                return t.wildcard();
            }
        };

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP
