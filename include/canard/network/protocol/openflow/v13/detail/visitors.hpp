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

    } // namespace detail

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_VISITORS_HPP
