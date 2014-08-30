#ifndef CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class hello
        : public detail::basic_openflow_message<hello>
    {
    public:
        static ofp_type const message_type = OFPT_HELLO;

        hello()
            : hello_{{OFP_VERSION, message_type, sizeof(detail::ofp_hello), get_xid()}}
        {
        }

        explicit hello(std::vector<std::uint32_t> bitmaps)
            : hello{{hello_elements::versionbitmap{std::move(bitmaps)}}}
        {
        }

        explicit hello(std::vector<hello_elements::variant> elements)
            : hello_{{OFP_VERSION, message_type, calc_length(elements), get_xid()}}
            , elements_(std::move(elements))
        {
        }

        auto header() const
            -> detail::ofp_header const&
        {
            return hello_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, hello_);
            auto visitor = hello_elements::encoding_visitor<Container>{container};
            boost::for_each(elements_, boost::apply_visitor(visitor));
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> hello
        {
            auto const h = detail::decode<detail::ofp_hello>(first, last);
            if (std::distance(first, last) != h.header.length - sizeof(detail::ofp_hello)) {
                throw 2;
            }

            auto elements = std::vector<hello_elements::variant>{};
            while (first != last) {
                elements.push_back(hello_elements::decode(first, last));
            }
            return hello{h, std::move(elements)};
        }

    private:
        hello(detail::ofp_hello const& hello, std::vector<hello_elements::variant> elements)
            : hello_(hello)
            , elements_(std::move(elements))
        {
        }

        static auto calc_length(std::vector<hello_elements::variant> const& elements)
            -> std::uint16_t
        {
            auto visitor = hello_elements::calculating_exact_length_visitor{};
            return boost::accumulate(
                      elements | boost::adaptors::transformed(boost::apply_visitor(visitor))
                    , std::uint16_t{sizeof(hello_)});
        }

    private:
        detail::ofp_hello hello_;
        std::vector<hello_elements::variant> elements_;
    };

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP
