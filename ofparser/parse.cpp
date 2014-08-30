#include <utility>
#include <boost/spirit/include/qi_parse.hpp>
#include "code_information.hpp"
#include "ofparser_grammer.hpp"
#include "parse.hpp"

namespace ofparser {

    auto parse(std::string const& input)
        -> std::pair<std::vector<struct_info>, std::vector<enum_info>>
    {
        auto it = input.begin();
        std::vector<struct_info> sinfos;
        std::vector<enum_info> einfos;
        boost::spirit::qi::phrase_parse(it, input.end()
                , ofparser_grammer<std::string::const_iterator>{sinfos, einfos}, boost::spirit::qi::ascii::space);
        return {std::move(sinfos), std::move(einfos)};
    }

} // namespace ofparser

