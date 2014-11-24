#ifndef CANARD_NETWORK_OPENFLOW_VISITORS_HPP
#define CANARD_NETWORK_OPENFLOW_VISITORS_HPP

#include <cstdint>
#include <boost/variant/static_visitor.hpp>

namespace canard {
namespace network {
namespace openflow {

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

    } // namespace detail

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_VISITORS_HPP
