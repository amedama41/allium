#ifndef CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/numeric.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/any_hello_element.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/length_utility.hpp>
#include <canard/network/protocol/openflow/v13/message/hello_elements.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    namespace hello_detail {

        template <class HelloElement>
        auto find(std::vector<any_hello_element> const& elements)
            -> boost::optional<HelloElement const&>
        {
            auto const it = boost::find_if(elements, [](any_hello_element const& element) {
                return element.type() == HelloElement::hello_element_type;
            });
            if (it == elements.end()) {
                return boost::none;
            }
            return any_cast<HelloElement>(*it);
        }

        auto get_version(std::vector<any_hello_element> const& elements, std::uint8_t const default_version)
            -> std::uint8_t
        {
            if (auto const versionbitmap = find<hello_elements::versionbitmap>(elements)) {
                return versionbitmap->max_support_version();
            }
            return default_version;
        }

    } // namespace hello_detail

    class hello
        : public v13_detail::basic_openflow_message<hello>
    {
    public:
        static protocol::ofp_type const message_type = protocol::OFPT_HELLO;

        explicit hello(std::uint8_t const version = protocol::OFP_VERSION)
            : hello_{{version, message_type, sizeof(v13_detail::ofp_hello), get_xid()}}
        {
        }

        explicit hello(std::vector<std::uint32_t> bitmaps)
            : hello{{hello_elements::versionbitmap{std::move(bitmaps)}}}
        {
        }

        explicit hello(std::vector<any_hello_element> elements)
            : hello_{
                v13_detail::ofp_header{
                      hello_detail::get_version(elements, protocol::OFP_VERSION)
                    , message_type, calc_length(elements), get_xid()
                }
              }
            , elements_(std::move(elements))
        {
        }

        auto support(std::uint8_t const version) const
            -> bool
        {
            if (auto const versionbitmap = find<hello_elements::versionbitmap>()) {
                return versionbitmap->support(version);
            }
            return this->version() >= version;
        }

        auto max_support_version() const
            -> std::uint8_t
        {
            if (auto const versionbitmap = find<hello_elements::versionbitmap>()) {
                return versionbitmap->max_support_version();
            }
            return version();
        }

        template <class HelloElement>
        auto find() const
            -> boost::optional<HelloElement const&>
        {
            return hello_detail::find<HelloElement>(elements_);
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return hello_.header;
        }

        using basic_openflow_message::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            detail::encode(container, hello_);
            boost::for_each(elements_, [&](any_hello_element const& hello_elem) {
                hello_elem.encode(container);
            });
            return container;
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> hello
        {
            auto const h = detail::decode<v13_detail::ofp_hello>(first, last);
            if (std::distance(first, last) != h.header.length - sizeof(v13_detail::ofp_hello)) {
                throw 2;
            }

            auto elements = std::vector<any_hello_element>{};
            while (first != last) {
                elements.push_back(any_hello_element::decode(first, last));
            }
            return hello{h, std::move(elements)};
        }

    private:
        hello(v13_detail::ofp_hello const& hello, std::vector<any_hello_element> elements)
            : hello_(hello)
            , elements_(std::move(elements))
        {
        }

        static auto calc_length(std::vector<any_hello_element> const& elements)
            -> std::uint16_t
        {
            using boost::adaptors::transformed;
            return boost::accumulate(elements | transformed([](any_hello_element const& hello_elem) {
                    return v13_detail::exact_length(hello_elem.length());
            }), std::uint16_t{sizeof(hello_)});
        }

    private:
        v13_detail::ofp_hello hello_;
        std::vector<any_hello_element> elements_;
    };

} // namespace messages

using messages::hello;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_HELLO_HPP
