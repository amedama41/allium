#ifndef OFPARSER_PREPROCESS_HPP
#define OFPARSER_PREPROCESS_HPP

#include <string>
#include <tuple>
#include <unordered_map>

namespace ofparser {

    auto preprocess(std::string const& input, std::string const& name)
        -> std::tuple<std::string, std::unordered_map<std::string, std::size_t>, std::vector<std::pair<std::string, std::string>>>;

} // namespace ofparser

#endif // OFPARSER_PREPROCESS_HPP
