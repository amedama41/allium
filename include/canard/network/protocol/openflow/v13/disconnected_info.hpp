#ifndef CANARD_NETWORK_OPENFLOW_V13_DISCONNECTED_INFO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_DISCONNECTED_INFO_HPP

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    class disconnected_info
    {
    public:
        explicit disconnected_info(boost::system::error_code ec)
            : ec_(ec)
        {
        }

        auto error() const
            -> boost::system::error_code
        {
            return ec_;
        }

    private:
        boost::system::error_code ec_;
    };

    template <class OStream>
    auto operator<<(OStream& os, disconnected_info const& info)
        -> OStream&
    {
        return os << "discoonnected: " << info.error().message() << std::endl;
    }

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_DISCONNECTED_INFO_HPP
