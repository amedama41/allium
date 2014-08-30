#include <algorithm>
#include <unordered_set>
#include <utility>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/wave.hpp>
#include <boost/wave/preprocessing_hooks.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include "preprocess.hpp"

namespace ofparser {

    namespace detail {

        template <class TokenString>
        static auto to_string(TokenString const& str)
            -> std::string
        {
            return std::string(str.begin(), str.end());
        }

    } // namespace detail

    class ignore_include_hooks
        : public boost::wave::context_policies::default_preprocessing_hooks
    {
    public:
        ignore_include_hooks(
                std::unordered_map<std::string, std::size_t>& struct_to_size,
                std::vector<std::pair<std::string, std::string>>& constant_macros)
            : struct_to_size_{struct_to_size}
            , constant_macros_{constant_macros}
        {
        }

        template <class Context>
        bool found_include_directive(Context const& ctx, std::string const&, bool include_next)
        {
            return true;
        }

        bool is_constant_integer(std::string const& str) const
        {
            if (macros_.find(str) != macros_.end()) {
                return false;
            }
            auto pos = std::size_t{};
            try {
                std::stoull(str, &pos, 0);
            }
            catch (std::invalid_argument& e) {
                return false;
            }
            return pos == str.size();
        }

        template <class Token, class Definition>
        void extract_constant_macro_info(Token const& name, Definition const& definition)
        {
            if (definition.size() != 1 || !is_constant_integer(detail::to_string(definition.front().get_value()))) {
                std::cout << name.get_value() << " is not constant" << std::endl;
                return;
            }
            constant_macros_.emplace_back(detail::to_string(name.get_value()), detail::to_string(definition.front().get_value()));
        }

        template <class Context, class Token, class Parameters, class Definition>
        void defined_macro(Context const&, Token const& name, bool is_functionlike
                , Parameters const& parameters, Definition const& definition, bool is_predefined)
        {
            if (is_predefined || name.get_value() == "OPENFLOW_OPENFLOW_H") {
                return;
            }

            macros_.emplace(detail::to_string(name.get_value()));
            if (not is_functionlike) {
                extract_constant_macro_info(name, definition);
                return;
            }

            std::cout << name.get_value();
            if (is_functionlike) {
                using boost::adaptors::transformed;
                std::cout << '(' << boost::algorithm::join(parameters | transformed([](auto&& token) { return token.get_value(); }), ", ") << ')';
            }
            std::cout << '\t';
            for (auto& token : definition) {
                std::cout << token.get_value();
            }
            std::cout << std::endl;
        }

        bool is_ofp_assert_ = false;

        template <class Arg>
        void extract_struct_size_info(Arg&& arg)
        {
            if (arg.size() != 10 || arg.front().get_value() != "sizeof") {
                using boost::adaptors::transformed;
                std::cout << "not sizeof assertion: "
                    << boost::algorithm::join(arg | transformed([](auto&& token) { return token.get_value(); }), "-") << std::endl;
                return;
            }
            struct_to_size_.emplace(
                      detail::to_string(std::next(arg.begin(), 4)->get_value())
                    , std::stoull(detail::to_string(arg.back().get_value())));
        }

        template <class Context, class Token, class Container, class Iterator>
        bool expanding_function_like_macro(Context const&
                , Token const& macrodef, std::vector<Token> const& formal_args, Container const& definition
                , Token const& macrocall, std::vector<Container> const& arguments
                , Iterator const& seqstart, Iterator const& seqend)
        {
            if (macrodef.get_value() == "OFP_ASSERT") {
                is_ofp_assert_ = true;
                extract_struct_size_info(arguments.front());
                return false;
            }
            std::cout << "macrodef: " << macrodef.get_value() << std::endl;
            std::cout << "macrocall: " << macrocall.get_value() << std::endl;
            return false;
        }

#if 0
        template <class Context, class Token, class Container>
        bool expanding_object_like_macro(Context const&, Token const& macro, Container const& definition, Token const& macrocall)
        {
            std::cout << macro.get_value() << std::endl;
            std::cout << '\t';
            for (auto& token : definition) {
                std::cout << token.get_value();
            }
            std::cout << '\n' << '\t';
            std::cout << macrocall.get_value() << std::endl;
            return false;
        }

        template <class Context, class Container>
        void expanded_macro(Context const&, Container const& result)
        {
            if (is_ofp_assert_) {
                is_ofp_assert_ = false;
                return;
            }
            std::cout << "result:" << std::endl;
            std::cout << '\t';
            for (auto& token : result) {
                std::cout << token.get_value();
            }
            std::cout << std::endl;
        }
#endif
    private:
        std::unordered_map<std::string, std::size_t>& struct_to_size_;
        std::vector<std::pair<std::string, std::string>>& constant_macros_;
        std::unordered_set<std::string> macros_;
    };

    auto preprocess(std::string const& input, std::string const& name)
        -> std::tuple<std::string, std::unordered_map<std::string, std::size_t>, std::vector<std::pair<std::string, std::string>>>
    {
        using lex_iterator_type = boost::wave::cpplexer::lex_iterator<
            boost::wave::cpplexer::lex_token<>
        >;

        using contex_type = boost::wave::context<
            std::string::const_iterator, lex_iterator_type
          , boost::wave::iteration_context_policies::load_file_to_string
          , ignore_include_hooks
        >;

        auto struct_to_size = std::unordered_map<std::string, std::size_t>{};
        auto constant_macros = std::vector<std::pair<std::string, std::string>>{};
        auto context_policy = ignore_include_hooks{struct_to_size, constant_macros};
        contex_type ctx{input.begin(), input.end(), name.c_str(), context_policy};

        ctx.set_language(boost::wave::support_cpp11);

        auto result = std::string{};
        result.reserve(input.size());
        std::for_each(ctx.begin(), ctx.end(), [&](contex_type::iterator_type::reference v) {
            result.append(v.get_value().begin(), v.get_value().end());
        });
        return std::make_tuple(result, std::move(struct_to_size), std::move(constant_macros));
    }

} // namespace ofparser

