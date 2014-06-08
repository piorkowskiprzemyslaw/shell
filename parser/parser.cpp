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
	parser::SUBSHELL,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::DOLLAR,
	(std::string, content)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::ASSIGNMENT_WORD,
    (std::string, var_name)
    (parser::STRING_EXPR, var_val)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION_IN_FILE,
	(parser::STRING_EXPR, file_name)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION_OUT_FILE,
	(parser::STRING_EXPR, file_name)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION_NUMBER_IN_FILE,
	(int, number)
	(parser::STRING_EXPR, file_name)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION_NUMBER_OUT_FILE,
	(int, number)
	(parser::STRING_EXPR, file_name)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION,
	(parser::redirection_type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::REDIRECTION_LIST,
	(parser::REDIRECTION, redirection)
	(std::vector<parser::REDIRECTION_LIST_NODE>, futher_redirections)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::SIMPLE_COMMAND,
	(parser::SIMPLE_COMMAND_ELEMENT, simple_command_element)
	(std::vector<parser::SIMPLE_COMMAND_NODE>, futher_simple_commands)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::SUBSHELL_REDIRECTION_LIST,
	(parser::SUBSHELL, subshell)
	(parser::REDIRECTION_LIST, redirection_list)
)

BOOST_FUSION_ADAPT_STRUCT(
	parser::PIPELINE,
	(parser::COMMAND, command)
	(std::vector<parser::PIPELINE_NODE>, pipeline_node)
)

/*
 * Grammar implementation...
 */
namespace parser {

	template<typename Iterator>
	struct mini_shell_grammar : qi::grammar<Iterator, PIPELINE(), ascii::space_type>
	{
		private:
			qi::rule<Iterator, std::string(), ascii::space_type> var_name;
			qi::rule<Iterator, TEXT(), ascii::space_type> text;
			qi::rule<Iterator, NUMBER(), ascii::space_type> number;
			qi::rule<Iterator, BACK_QUOTE(), ascii::space_type> back_quote;
			qi::rule<Iterator, QUOTE(), ascii::space_type> quote;
			qi::rule<Iterator, DOLLAR(), ascii::space_type> dollar;
			qi::rule<Iterator, STRING_EXPR(), ascii::space_type> string_expr;

			qi::rule<Iterator, ASSIGNMENT_WORD(), ascii::space_type> assignment_word;

			qi::rule<Iterator, REDIRECTION_IN_FILE(), ascii::space_type> redirection_in_file;
			qi::rule<Iterator, REDIRECTION_OUT_FILE(), ascii::space_type> redirection_out_file;
			qi::rule<Iterator, REDIRECTION_NUMBER_IN_FILE(), ascii::space_type> redirection_number_in_file;
			qi::rule<Iterator, REDIRECTION_NUMBER_OUT_FILE(), ascii::space_type> redirection_number_out_file;
			qi::rule<Iterator, REDIRECTION(), ascii::space_type> redirection;
			qi::rule<Iterator, REDIRECTION_LIST_NODE(), ascii::space_type> redirection_list_node;
			qi::rule<Iterator, REDIRECTION_LIST(), ascii::space_type> redirection_list;

			qi::rule<Iterator, SIMPLE_COMMAND_ELEMENT(), ascii::space_type> simple_command_element;
			qi::rule<Iterator, SIMPLE_COMMAND_NODE(), ascii::space_type> simple_command_node;
			qi::rule<Iterator, SIMPLE_COMMAND(), ascii::space_type> simple_command;

			qi::rule<Iterator, SUBSHELL(), ascii::space_type> subshell;
			qi::rule<Iterator, SUBSHELL_REDIRECTION_LIST(), ascii::space_type> subshell_redirection_list;
			qi::rule<Iterator, COMMAND(), ascii::space_type> command;

			qi::rule<Iterator, PIPELINE_NODE(), ascii::space_type> pipeline_node;
			qi::rule<Iterator, PIPELINE(), ascii::space_type> pipeline;


		public:
			mini_shell_grammar() : mini_shell_grammar::base_type(pipeline) {
				using qi::lit;
				using qi::lexeme;
				using ascii::char_;
				using ascii::space;
				using boost::spirit::int_;
				using ascii::string;
				using boost::spirit::qi::eps;
				using namespace qi::labels;

				/*
				 * Basic grammar stuff...
				 */
				var_name %= lexeme[ !(char_("0-9")) >> +char_("0-9a-zA-Z") ];
				text %= lexeme[ +( char_ - space - lit('`') - lit('\'') - lit('(') - lit(')') - lit('$') - lit('|') -lit('&') - lit(';') ) ];
				number %= int_;
				back_quote %= lit('`') >> lexeme[ +(char_ - '`')] >> lit('`');
				quote %= lit('\'') >> lexeme[ +(char_ - '\'')] >> lit('\'');
				dollar %= lit('$') >> var_name;
				string_expr %= number | back_quote | quote | dollar | text;

				/*
				 * Assignment...
				 */
				assignment_word %= var_name >> lit('=') >> string_expr;

				/*
				 * Redirection...
				 */
				redirection_in_file %= lit('<') >> string_expr;
				redirection_out_file %= lit('>') >> string_expr;
				redirection_number_in_file %= number >> lit('<') >> string_expr;
				redirection_number_out_file %= number >> lit('>') >> string_expr;
				redirection %= redirection_in_file | redirection_out_file | redirection_number_in_file | redirection_number_out_file;
				redirection_list_node %= redirection | redirection_list;
				redirection_list %= redirection >> *redirection_list_node;

				/*
				 * simple command element...
				 */
				simple_command_element %= redirection | assignment_word |  string_expr;
				simple_command_node %= simple_command_element | simple_command;
				simple_command %= simple_command_element >> *simple_command_node;

				/*
				 * command...
				 */
				subshell %= lit('(') >> lexeme[ +(char_ - ')' - '(')] >> lit(')');
				subshell_redirection_list %= subshell >> redirection_list;
				command %= simple_command | subshell | subshell_redirection_list; // moze wymagac zamiany miejscami ( ostatnie dwa elementy alternatywy )

				/*
				 * pipeline...
				 */
				pipeline_node %= command | pipeline;
				pipeline %= command >> *( lit('|') >> pipeline_node );
			}

	};

	int parse(const std::string & inputString, PIPELINE & toks) {
		using boost::spirit::ascii::space;
		std::string::const_iterator begin = inputString.begin();
		std::string::const_iterator end = inputString.end();
		mini_shell_grammar<std::string::const_iterator> shell;
		bool r = phrase_parse(begin, end, shell, space, toks);

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
