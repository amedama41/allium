#ifndef CANARD_EXCEPTION_HPP
#define CANARD_EXCEPTION_HPP

#include <string>
#include <boost/system/error_code.hpp>

namespace canard {

    enum errc_t
    {
        has_any_exception = 1
    };

    namespace detail {

        class canard_error_category
            : public boost::system::error_category
        {
            auto name() const noexcept
                -> char const*
            {
                return "canard_error_category";
            }

            auto message(int const e) const
                -> std::string
            {
                switch (e) {
                case has_any_exception:
                    return "Has any exception: 1";
                default:
                    return "Undefined error: 0";
                }
            }
        };

    } // namespace detail

    inline auto canard_error_category()
        -> boost::system::error_category&
    {
        static detail::canard_error_category instance{};
        return instance;
    }

} // namespace canard

namespace boost {
namespace system {

    template <> struct is_error_code_enum<canard::errc_t>
    {
        static bool const value = true;
    };

} // namespace system
} // namespace boost

namespace canard {

    inline auto make_error_code(canard::errc_t const e)
        -> boost::system::error_code
    {
        return boost::system::error_code{
            e, canard_error_category()
        };
    }

} // namespace canard

#endif // CANARD_EXCEPTION_HPP
