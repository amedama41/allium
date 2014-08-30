#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <cstdint>
#include <boost/variant/variant.hpp>

struct constant_expr
{
    boost::variant<std::uint64_t, std::int64_t> value;

    template <class T>
    T value() const
    {
        return value;
    }
};

struct expr;

using expr_node = boost::variant<constant_expr, expr>;

struct expr
{
    std::string op;
    std::vector<expr_node> operands;

    template <class T>
    T value() const
    {
    }
};

#endif // EXPRESSION_HPP
