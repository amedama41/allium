#include <regex>
#include "comment_to_space.hpp"

namespace ofparser {

    auto comment_to_space(std::string const& input)
        -> std::string
    {
        return std::regex_replace(input, std::regex{R"(/\*.*\*/)"}, " ");
    }

} // namespace ofparser

