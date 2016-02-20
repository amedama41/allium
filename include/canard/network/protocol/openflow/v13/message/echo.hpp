#ifndef CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ECHO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ECHO_HPP

#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <boost/range/iterator_range.hpp>
#include <canard/network/protocol/openflow/binary_data.hpp>
#include <canard/network/protocol/openflow/detail/decode.hpp>
#include <canard/network/protocol/openflow/detail/encode.hpp>
#include <canard/network/protocol/openflow/get_xid.hpp>
#include <canard/network/protocol/openflow/v13/detail/basic_openflow_message.hpp>
#include <canard/network/protocol/openflow/v13/detail/byteorder.hpp>
#include <canard/network/protocol/openflow/v13/openflow.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {
namespace messages {

    namespace echo_detail {

        template <class T>
        class echo_base
            : public v13_detail::basic_openflow_message<T>
        {
        public:
            using data_type = binary_data::pointer_type;

        protected:
            echo_base(binary_data&& data, std::uint32_t const xid) noexcept
                : header_{
                      protocol::OFP_VERSION
                    , T::message_type
                    , std::uint16_t(
                            sizeof(v13_detail::ofp_header) + data.size())
                    , xid
                  }
                , data_(std::move(data).data())
            {
            }

            echo_base(v13_detail::ofp_header const& header, data_type&& data)
                : header_(header)
                , data_(std::move(data))
            {
            }

            echo_base(echo_base const& other)
                : header_(other.header_)
                , data_(binary_data::copy_data(other.data()))
            {
            }

            echo_base(echo_base&& other) noexcept
                : header_(other.header_)
                , data_(std::move(other.data_))
            {
                other.header_.length = sizeof(v13_detail::ofp_header);
            }

            auto operator=(echo_base const& other)
                -> echo_base&
            {
                auto tmp = other;
                return operator=(std::move(tmp));
            }

            auto operator=(echo_base&& other) noexcept
                -> echo_base&
            {
                auto tmp = std::move(other);
                header_ = tmp.header_;
                data_ = std::move(tmp.data_);
                return *this;
            }

        public:
            auto header() const noexcept
                -> v13_detail::ofp_header const&
            {
                return header_;
            }

            auto data() const noexcept
                -> boost::iterator_range<unsigned char const*>
            {
                return boost::make_iterator_range_n(data_.get(), data_length());
            }

            auto data_length() const noexcept
                -> std::uint16_t
            {
                return this->length() - sizeof(v13_detail::ofp_header);
            }

            auto extract_data() noexcept
                -> binary_data
            {
                auto const data_len = data_length();
                header_.length = sizeof(v13_detail::ofp_header);
                return binary_data{std::move(data_), data_len};
            }

            template <class Container>
            auto encode(Container& container) const
                -> Container&
            {
                detail::encode(container, header_);
                return detail::encode_byte_array(
                        container, data_.get(), data_length());
            }

            template <class Iterator>
            static auto decode(Iterator& first, Iterator last)
                -> T
            {
                auto const header
                    = detail::decode<v13_detail::ofp_header>(first, last);
                last = std::next(
                        first, header.length - sizeof(v13_detail::ofp_header));
                auto data = binary_data::copy_data(first, last);
                first = last;

                return T{header, std::move(data)};
            }

            static void validate(v13_detail::ofp_header const& header)
            {
                if (header.version != protocol::OFP_VERSION) {
                    throw std::runtime_error{"invalid version"};
                }
                if (header.type != T::message_type) {
                    throw std::runtime_error{"invalid message type"};
                }
                if (header.length < sizeof(v13_detail::ofp_header)) {
                    throw std::runtime_error{"invalid length"};
                }
            }

        private:
            v13_detail::ofp_header header_;
            data_type data_;
        };

    } // namespace echo_detail


    class echo_request
        : public echo_detail::echo_base<echo_request>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_ECHO_REQUEST;

        explicit echo_request(
                binary_data data, std::uint32_t const xid = get_xid()) noexcept
            : echo_base{std::move(data), xid}
        {
        }

        explicit echo_request(std::uint32_t const xid = get_xid()) noexcept
            : echo_request{binary_data{}, xid}
        {
        }

    private:
        friend echo_base;

        echo_request(v13_detail::ofp_header const& header
                   , echo_base::data_type&& data) noexcept
            : echo_base{header, std::move(data)}
        {
        }
    };


    class echo_reply
        : public echo_detail::echo_base<echo_reply>
    {
    public:
        static constexpr protocol::ofp_type message_type
            = protocol::OFPT_ECHO_REPLY;

        explicit echo_reply(
                binary_data data, std::uint32_t const xid = get_xid()) noexcept
            : echo_base{std::move(data), xid}
        {
        }

        explicit echo_reply(std::uint32_t const xid = get_xid()) noexcept
            : echo_reply{binary_data{}, xid}
        {
        }

        explicit echo_reply(echo_request request) noexcept
            : echo_reply{request.extract_data(), request.xid()}
        {
        }

    private:
        friend echo_base;

        echo_reply(v13_detail::ofp_header const& header
                 , echo_base::data_type&& data) noexcept
            : echo_base{header, std::move(data)}
        {
        }
    };

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_MESSAGES_ECHO_HPP
