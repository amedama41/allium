#ifndef CANARD_NETWORK_OPENFLOW_GOODBYE_HPP
#define CANARD_NETWORK_OPENFLOW_GOODBYE_HPP

#include <boost/system/error_code.hpp>

namespace canard {
namespace network {
namespace openflow {

    class goodbye
    {
    public:
        explicit goodbye(boost::system::error_code const ec)
            : ec_(ec)
        {
        }

        auto reason() const
            -> boost::system::error_code
        {
            return ec_;
        }

    private:
        boost::system::error_code ec_;
    };

} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_GOODBYE_HPP
