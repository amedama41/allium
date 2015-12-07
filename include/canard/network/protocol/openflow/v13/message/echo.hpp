#ifndef CANARD_NETWORK_OPENFLOW_V13_ECHO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_ECHO_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/decode.hpp>
#include <canard/network/protocol/openflow/v13/detail/encode.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    template <class T>
    class basic_echo
        : public v13_detail::basic_openflow_message<T>
    {
        using v13_detail::basic_openflow_message<T>::get_xid;

    protected:
        basic_echo()
            : header_{
                  protocol::OFP_VERSION, T::message_type
                , sizeof(v13_detail::ofp_header), get_xid()
              }
        {
        }

        explicit basic_echo(std::vector<unsigned char> data)
            : header_{
                  protocol::OFP_VERSION, T::message_type
                , std::uint16_t(sizeof(v13_detail::ofp_header) + data.size())
                , get_xid()
              }
            , data_(std::move(data))
        {
        }

        template <class>
        friend class basic_echo;

        template <class U>
        basic_echo(basic_echo<U> const& echo2)
            : header_{echo2.version(), T::message_type, echo2.length(), echo2.xid()}
            , data_(echo2.data_)
        {
        }

        template <class U>
        basic_echo(basic_echo<U>&& echo2)
            : header_{echo2.version(), T::message_type, echo2.length(), echo2.xid()}
            , data_(std::move(echo2.data_))
        {
            echo2.header_.length = sizeof(v13_detail::ofp_header) + echo2.data_.size();
        }

        basic_echo(v13_detail::ofp_header const& header, std::vector<unsigned char> data)
            : header_(header)
            , data_(std::move(data))
        {
            if (header_.length != sizeof(header_) + data_.size()) {
                throw 1;
            }
        }

    public:
        auto data() const
            -> std::vector<unsigned char> const&
        {
            return data_;
        }

        auto header() const
            -> v13_detail::ofp_header const&
        {
            return header_;
        }

        using v13_detail::basic_openflow_message<T>::encode;

        template <class Container>
        auto encode(Container& container) const
            -> Container&
        {
            v13_detail::encode(container, header_);
            return boost::push_back(container, data_);
        }

    protected:
        template <class Iterator>
        static auto decode_impl(Iterator& first, Iterator last)
            -> std::tuple<v13_detail::ofp_header, std::vector<unsigned char>>
        {
            auto const header = v13_detail::decode<v13_detail::ofp_header>(first, last);
            if (header.length != sizeof(v13_detail::ofp_header) + std::distance(first, last)) {
                throw 2;
            }
            auto data = std::vector<unsigned char>(first, last);
            std::advance(first, data.size());

            return std::make_tuple(header, std::move(data));
        }

    private:
        v13_detail::ofp_header header_;
        std::vector<unsigned char> data_;
    };

    class echo_request;

    class echo_reply
        : public basic_echo<echo_reply>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_ECHO_REPLY;

        echo_reply()
            : basic_echo{}
        {
        }

        explicit echo_reply(std::vector<unsigned char> data)
            : basic_echo{std::move(data)}
        {
        }

        explicit echo_reply(echo_request const& request);
        explicit echo_reply(echo_request&& request);

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> echo_reply
        {
            auto header_and_data = decode_impl(first, last);
            return echo_reply{std::get<0>(header_and_data), std::move(std::get<1>(header_and_data))};
        }

    private:
        echo_reply(v13_detail::ofp_header const& header, std::vector<unsigned char> data)
            : basic_echo(header, std::move(data))
        {
            if (type() != message_type) {
                throw 1;
            }
        }
    };

    class echo_request
        : public basic_echo<echo_request>
    {
    public:
        static protocol::ofp_type const message_type
            = protocol::OFPT_ECHO_REQUEST;

        echo_request()
            : basic_echo{}
        {
        }

        explicit echo_request(std::vector<unsigned char> data)
            : basic_echo{std::move(data)}
        {
        }

        auto reply() const&
            -> echo_reply
        {
            return echo_reply{*this};
        }

        auto reply() &&
            -> echo_reply
        {
            return echo_reply{std::move(*this)};
        }

        template <class Iterator>
        static auto decode(Iterator& first, Iterator last)
            -> echo_request
        {
            auto header_and_data = decode_impl(first, last);
            return echo_request{std::get<0>(header_and_data), std::move(std::get<1>(header_and_data))};
        }

    private:
        echo_request(v13_detail::ofp_header const& header, std::vector<unsigned char> data)
            : basic_echo(header, std::move(data))
        {
            if (type() != message_type) {
                throw 1;
            }
        }
    };

    inline echo_reply::echo_reply(echo_request const& request)
        : basic_echo{request}
    {
    }

    inline echo_reply::echo_reply(echo_request&& request)
        : basic_echo{std::move(request)}
    {
    }

} // namespace messages

using messages::echo_request;
using messages::echo_reply;

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_ECHO_HPP
