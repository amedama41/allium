#ifndef OFPARSER_OFPARSER_GRAMMER_HPP
#define OFPARSER_OFPARSER_GRAMMER_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "code_information.hpp"

namespace ofparser {
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;

    template <class Iterator>
    struct ofparser_grammer : qi::grammar<Iterator, std::string(), qi::ascii::space_type>
    {
        ofparser_grammer(std::vector<struct_info>& sinfo, std::vector<enum_info>& einfo)
            : ofparser_grammer::base_type{start, "translation-unit"}
            , struct_infos_{&sinfo}
            , enum_infos_{&einfo}
        {
            using qi::alnum;
            using qi::alpha;
            using qi::ascii::string;
            using qi::ascii::char_;
            using qi::_val;

            keywords
                =   sizeof_
                |   storage_class_specifier
                |   types_
                |   struct_or_union
                |   enum_
                |   type_qualifier
                |   function_specifier
                ;

            identifer
                =   ( ( alpha | char_('_') ) >> *( alnum | char_('_') ) )
                    -   keywords
                ;

            init_expression();
            init_declaration();

            start
                =  +declaration [qi::_val += qi::_1 + '\n']
                ;

            qi::on_error<qi::fail>
            (
                declaration
              , std::cout
                    << phoenix::val("Error! Expecting ")
                    << qi::labels::_4
                    << "  here: \""
                    << phoenix::construct<std::string>(qi::labels::_3, qi::labels::_2)
                    << "\""
                    << std::endl
            );

            add_preserve_types();
        }

        void init_expression()
        {
            using qi::alnum;
            using qi::lit;
            using qi::ascii::string;
            using qi::ascii::char_;
            using qi::raw;
            using qi::oct;
            using qi::hex;
            using qi::ulong_long;
            using qi::_val;
            using qi::labels::_1;
            using phoenix::at_c;

            constant
                =   integer_constant            [_val = _1]
                // |   floating_constant
                |   enumerator_constant         [([this](auto& enumrator, auto& ctx, bool& pass) {
                                                    pass = (enum_constants_.find(enumrator) != enum_constants_.end());
                                                    at_c<0>(ctx.attributes) = enumrator;
                                                })]
                // |   character_constant
                ;

            integer_constant
                %=  raw[
                        (   ( ( lit("0x") | "0X" ) >> hex )
                        |   ( lit("0") >> oct )
                        |   ( ulong_long )
                        )
                    ]
                    >> -qi::omit[ integer_suffix ]
                ;

            integer_suffix
                =   unsigned_suffix ^ ( long_long_suffix | long_suffix )
                ;

            unsigned_suffix
                =   lit('u') | 'U'
                ;

            long_suffix
                =   lit('l') | 'L'
                ;

            long_long_suffix
                =   lit("ll") | "LL"
                ;

            enumerator_constant
                =   identifer
                ;

            primary_expression
                =   constant
                |   identifer
                // |   string_litral
                |   '(' >> expression >> ')'
                ;

            postfix_expression
                =   primary_expression          [_val = _1]
                    >> *qi::as_string
                        [ ( '[' >> expression >> ']' )
                        | ( '(' >> -argument_expression_list >> ')' )
                        | ( '.' >> identifer )
                        | ( "->" >> identifer )
                        | ( "++" )
                        | ( "--" )
                        | ( '(' >> type_name >> ')' >> '{' >> initializer_list >> -lit(',') >> '}' )
                        ]                       [_val += _1]
                ;

            argument_expression_list
                =   assignment_expression % ','
                ;

            unary_expression
                =   postfix_expression
                |   "++" >> unary_expression
                |   "--" >> unary_expression
                |   unary_operator >> cast_expression
                |   string("sizeof") >> '(' >> type_name >> ')'
                |   string("sizeof") >> unary_expression
                ;

            sizeof_
                =   string("sizeof")
                    - !( alnum | '_' )
                ;

            unary_operator
                =   char_('&')
                |   char_('*')
                |   char_('+')
                |   char_('-')
                |   char_('~')
                |   char_('!')
                ;

            cast_expression
                =   '(' >> type_name >> ')' >> cast_expression
                |   unary_expression
                ;

            multiplicative_expression
                =   cast_expression
                    >> *(   ( char_('*') | '/' | '%' )
                            >> cast_expression
                        )
                ;

            additive_expression
                =   multiplicative_expression
                    >> *(   ( char_('+') | '-' )
                            >> multiplicative_expression
                        )
                ;

            shift_expression
                =   additive_expression                     [_val = _1]
                    >> *(   ( string("<<") | string(">>") ) [_val += _1]
                            >> additive_expression          [_val += _1]
                        )
                ;

            relational_expression
                =   shift_expression
                    >> *(   ( char_('<') | '>' | "<=" | ">=" )
                            >> shift_expression
                        )
                ;

            equality_expression
                =   relational_expression
                    >> *(   ( string("==") | string("!=") )
                            >> relational_expression
                        )
                ;

            and_expression
                =   equality_expression
                    >> -( '&' >> equality_expression )
                ;

            exclusive_or_expression
                =   and_expression
                    >> -( '^' >> and_expression )
                ;

            inclusive_or_expression
                =   exclusive_or_expression
                    >> -( '|' >> exclusive_or_expression )
                ;

            logical_and_expression
                =   inclusive_or_expression
                    >> -( "&&" >> inclusive_or_expression )
                ;

            logical_or_expression
                =   logical_and_expression
                    >> -( "||" >> logical_and_expression )
                ;

            conditional_expression
                =   logical_or_expression
                    >> -( '?' >> expression >> ':' >> conditional_expression )
                ;

            assignment_expression
                =   unary_expression
                    >> assignment_operator
                    >> assignment_expression
                |   conditional_expression
                ;

            assignment_operator
                =   char_('=')
                |   string("*=")
                |   string("/=")
                |   string("%=")
                |   string("+=")
                |   string("-=")
                |   string("<<=")
                |   string(">>=")
                |   string("&=")
                |   string("^=")
                |   string("|=")
                ;

            expression
                =   assignment_expression
                    >> *( ',' >> assignment_expression )
                ;

            constant_expression
                =   conditional_expression
                ;
        }

        void init_declaration()
        {
            using qi::alnum;
            using qi::lit;
            using qi::ascii::string;
            using qi::_val;
            using qi::labels::_1;
            using phoenix::at_c;

            declaration
                =   declaration_specifiers
                    >> -init_declaratior_list
                    >   ';'
                ;

            declaration_specifiers
                =   storage_class_specifier
                    >> -declaration_specifiers
                |   type_specifier
                    >> -declaration_specifiers
                |   type_qualifier
                    >> -declaration_specifiers
                |   function_specifier
                    >> -declaration_specifiers
                ;

            init_declaratior_list
                =   init_declaratior % ','
                ;

            init_declaratior
                =   declarator                  [_val = at_c<0>(_1)]
                    >> -( '=' >> initializer )
                ;

            storage_class_specifier
                =   (   string("typedef")
                    |   string("extern")
                    |   string("static")
                    |   string("auto")
                    |   string("register")
                    )
                    >> -( alnum | '_' )
                ;

            type_specifier
                =   types_                      [_val = _1]
                |   struct_or_union_specifier   [([this](auto& sinfo, auto& ctx, auto&) {
                                                    at_c<0>(ctx.attributes) = sinfo.name;
                                                })]
                |   enum_specifier              [_val = at_c<0>(_1)]
                |   typedef_name                [_val = _1]
                ;

            types_
                =   (   string("void")
                    |   string("char")
                    |   string("short")
                    |   string("int")
                    |   string("long")
                    |   string("float")
                    |   string("double")
                    |   string("signed")
                    |   string("unsigned")
                    |   string("_Bool")
                    |   string("_Complex")
                    )
                    >> -( alnum | '_' )
                ;

            struct_or_union_specifier
                =   (   struct_or_union
                        >> -identifer           [at_c<0>(_val) = _1]
                        >>  '{'
                        >>  struct_declaration_list
                                                [at_c<1>(_val) = _1]
                        >>  '}'
                    )                           [([this](auto& sinfo, auto& ctx, auto&){
                                                    (*struct_infos_).push_back(at_c<0>(ctx.attributes));
                                                })]
                |   struct_or_union
                    >> identifer                [at_c<0>(_val) = _1]
                ;

            struct_or_union
                =   (   string("struct")
                    |   string("union")
                    )
                    >> !( alnum | '_' )
                ;

            struct_declaration_list
                =  +struct_declaration
                ;

            struct_declaration
                =   specifier_qualifier_list    [at_c<0>(_val) = _1]
                    >>  struct_declaratior      [at_c<1>(_val) = _1]
                        % ','
                    >   ';'
                ;

            specifier_qualifier_list
                =  +(   type_specifier
                    |   type_qualifier
                    )
                ;

            struct_declaratior
                =  -declarator                  [_val = _1]
                    >> -( ':' >> constant_expression )
                ;

            enum_specifier
                =   (   enum_
                        >> -identifer           [at_c<0>(_val) = _1]
                        >>  '{'
                        >>  enumerator_list     [at_c<1>(_val) = _1]
                        >> -lit(',')
                        >>  '}'
                    )                           [([this](auto& sinfo, auto& ctx, auto&){
                                                    (*enum_infos_).push_back(at_c<0>(ctx.attributes));
                                                })]
                |   enum_
                    >>  identifer               [at_c<0>(_val) = _1]
                ;

            enum_
                =   string("enum")
                    >> !( alnum | '_' )
                ;

            enumerator_list
                %=  enumerator                  [([this](auto& attr, auto&, auto&){
                                                    enum_constants_.emplace(attr.name, attr.value);
                                                })]
                    % ','
                ;

            enumerator
                %=  enumerator_constant
                    >> -( '=' >> constant_expression )
                ;

            type_qualifier
                =   (   string("const")
                    |   string("restrict")
                    |   string("volatile")
                    )
                    >> !( alnum | '_' )
                ;

            function_specifier
                =   string("inline")
                    >> !( alnum | '_' )
                ;

            declarator
                =  -pointer >> direct_declarator[_val = _1]
                ;

            direct_declarator
                =   (   identifer               [at_c<0>(_val) = _1]
                    |   '('
                        >> declarator           [_val = _1]
                        >> ')'
                    )
                    >> *(   ( '[' >> assignment_expression >> ']' )
                                                [([](auto& n, auto& ctx, auto&) {
                                                    at_c<0>(ctx.attributes).is_array = true;
                                                    at_c<0>(ctx.attributes).array_length = std::stoul(n);
                                                })]
                        |   ( '(' >> ( parameter_type_list | -identifer_list ) >> ')' )
                        )
                ;

            pointer
                =  +('*' >> -type_qualifier_list)
                ;

            type_qualifier_list
                =  +type_qualifier
                ;

            parameter_type_list
                =   parameter_list
                    >> -( lit(',') >> "..." )
                ;

            parameter_list
                =   parameter_delcaration % ','
                ;

            parameter_delcaration
                =   declaration_specifiers >> qi::omit[ declarator ]
                |   declaration_specifiers >> -abstract_declarator
                ;

            identifer_list
                =   identifer % ','
                ;

            type_name
                =   specifier_qualifier_list
                    >> -abstract_declarator
                ;

            abstract_declarator
                =  -pointer >> direct_abstract_declarator
                |   pointer
                ;

            direct_abstract_declarator
                =   (   '(' >> abstract_declarator >> ')'
                    >> *(   ( '[' >> -assignment_expression >> ']' )
                        |   ( '(' >> -parameter_type_list >> ')' )
                        )
                    )
                |  +(   ( '[' >> -assignment_expression >> ']' )
                    |   ( '(' >> -parameter_type_list >> ')' )
                    )
                ;

            typedef_name
                %=  identifer                   [([this](auto& name, auto&, bool& pass) {
                                                    pass = (typedef_names_.find(name) != typedef_names_.end());
                                                })]
                ;

            initializer
                =   assignment_expression
                |   '{'
                    >>  initializer_list
                    >> -lit(',')
                    >>  '}'
                ;

            initializer_list
                =   ( -designation >> initializer ) % ','
                ;

            designation
                =   designator_list >> '='
                ;

            designator_list
                =  +designator
                ;

            designator
                =   '[' >> constant_expression >> ']'
                |   '.' >> identifer
                ;
        }

        void add_preserve_types()
        {
            typedef_names_.emplace("uint8_t", true);
            typedef_names_.emplace("uint16_t", true);
            typedef_names_.emplace("uint32_t", true);
            typedef_names_.emplace("uint64_t", true);
        }

        qi::rule<Iterator, std::string()> constant;
        qi::rule<Iterator, std::string()> integer_constant;
        qi::rule<Iterator, std::string()> integer_suffix;
        qi::rule<Iterator, std::string()> unsigned_suffix;
        qi::rule<Iterator, std::string()> long_suffix;
        qi::rule<Iterator, std::string()> long_long_suffix;
        qi::rule<Iterator, std::string()> enumerator_constant;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> primary_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> postfix_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> argument_expression_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> unary_expression;
        qi::rule<Iterator, std::string()> sizeof_;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> unary_operator;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> cast_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> multiplicative_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> additive_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> shift_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> relational_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> equality_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> and_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> exclusive_or_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> inclusive_or_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> logical_and_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> logical_or_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> conditional_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> assignment_expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> assignment_operator;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> expression;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> constant_expression;

        qi::rule<Iterator, std::string(), qi::ascii::space_type> declaration;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> declaration_specifiers;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> init_declaratior_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> init_declaratior;
        qi::rule<Iterator, std::string()> storage_class_specifier;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> type_specifier;
        qi::rule<Iterator, std::string()> types_;
        qi::rule<Iterator, struct_info(), qi::ascii::space_type> struct_or_union_specifier;
        qi::rule<Iterator, std::string()> struct_or_union;
        qi::rule<Iterator, std::vector<declaration_info>(), qi::ascii::space_type> struct_declaration_list;
        qi::rule<Iterator, declaration_info(), qi::ascii::space_type> struct_declaration;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> specifier_qualifier_list;
        qi::rule<Iterator, declarator_info(), qi::ascii::space_type> struct_declaratior;
        qi::rule<Iterator, enum_info(), qi::ascii::space_type> enum_specifier;
        qi::rule<Iterator, std::string()> enum_;
        qi::rule<Iterator, std::vector<enum_member_info>(), qi::ascii::space_type> enumerator_list;
        qi::rule<Iterator, enum_member_info(), qi::ascii::space_type> enumerator;
        qi::rule<Iterator, std::string()> type_qualifier;
        qi::rule<Iterator, std::string()> function_specifier;
        qi::rule<Iterator, declarator_info(), qi::ascii::space_type> declarator;
        qi::rule<Iterator, declarator_info(), qi::ascii::space_type> direct_declarator;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> pointer;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> type_qualifier_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> parameter_type_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> parameter_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> parameter_delcaration;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> identifer_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> type_name;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> abstract_declarator;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> direct_abstract_declarator;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> typedef_name;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> initializer;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> initializer_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> designation;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> designator_list;
        qi::rule<Iterator, std::string(), qi::ascii::space_type> designator;

        qi::rule<Iterator, std::string()> keywords;
        qi::rule<Iterator, std::string()> identifer;

        qi::rule<Iterator, std::string(), qi::ascii::space_type> start;

        std::unordered_map<std::string, bool> typedef_names_;
        std::unordered_map<std::string, std::string> enum_constants_;

        std::vector<struct_info>* struct_infos_;
        std::vector<enum_info>* enum_infos_;
    };

} // namespace ofparser
#endif // OFPARSER_OFPARSER_GRAMMER_HPP
