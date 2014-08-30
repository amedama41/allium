#ifndef OFPARSER_PARSE_HPP
#define OFPARSER_PARSE_HPP

#include <string>
#include <utility>
#include <vector>
#include "code_information.hpp"

namespace ofparser {

    auto parse(std::string const& input)
        -> std::pair<std::vector<struct_info>, std::vector<enum_info>>;

} // namespace ofparser

#endif // OFPARSER_PARSE_HPP
