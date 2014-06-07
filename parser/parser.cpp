/*
 * parser.cpp
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#include <parser/parser.h>

/*
 * Adapting structs...
 */
BOOST_FUSION_ADAPT_STRUCT(
	parser::BACK_QUOTE,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::QUOTE,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::BRACKETS,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::DOLLAR,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::tokens,
    (std::vector<parser::node>, toks)
)

/*
 * Grammar implementation...
 */
namespace parser {

	template<typename Iterator>
	struct mini_shell_grammar : qi::grammar<Iterator, tokens()>
	{
		private:
			qi::rule<Iterator, WORD()> word;
			qi::rule<Iterator, NUMBER()> number;
			qi::rule<Iterator, BACK_QUOTE()> back_quote;
			qi::rule<Iterator, QUOTE()> quote;
			qi::rule<Iterator, BRACKETS()> brackets;
			qi::rule<Iterator, DOLLAR()> dollar;
			qi::rule<Iterator, tokens()> start;

		public:
			mini_shell_grammar() : mini_shell_grammar::base_type(start)
			{
				using qi::lit;
				using qi::lexeme;
				using ascii::char_;
				using boost::spirit::int_;
				using ascii::string;
				using boost::spirit::qi::eps;
				using namespace qi::labels;

				word %= +char_("a-zA-Z");
				number %= int_;
				back_quote %= lit('`') >> lexeme[ +(char_ - '`')] >> lit('`');
				quote %= lit('\'') >> lexeme[ +(char_ - '\'')] >> lit('\'');
				brackets %= lit('(') >> lexeme[ +(char_ - ')')] >> lit(')');
				dollar %= lit('$') >> word;
				start %= *lit(' ') >> ( word | number | back_quote | quote | brackets | dollar ) % lit(' ') >> (*lit(' ') ^ *lit('\n') );
			}

	};

	int parse(const std::string & inputString, tokens & toks) {
		using boost::spirit::ascii::space;
		std::string::const_iterator begin = inputString.begin();
		std::string::const_iterator end = inputString.end();
		mini_shell_grammar<std::string::const_iterator> shell;
		bool r = parse(begin, end, shell, toks);

	   if (r && begin == end)
		{
			return 1;
		}
		else
		{
			std::string::const_iterator some = begin+30;
			std::string context(begin, (some>end)?end:some);
			std::cout << "-------------------------\n";
			std::cout << "Parsing failed\n";
			std::cout << "stopped at :" << context << "\n";
			std::cout << "-------------------------\n";
			return 0;
		}


	}

}
