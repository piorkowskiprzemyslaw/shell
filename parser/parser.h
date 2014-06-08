/*
 * parser.h
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace parser {
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	/*
	 * Grammar stuff goes here...
	 */
	typedef std::string TEXT;
	typedef int NUMBER;
	struct BACK_QUOTE {
		std::string content;
	};
	struct QUOTE {
		std::string content;
	};
	struct BRACKETS {
		std::string content;
	};
	struct DOLLAR {
		std::string content;
	};

	typedef boost::variant< TEXT , NUMBER , BACK_QUOTE, QUOTE, BRACKETS, DOLLAR> STRING_EXPR;

	struct ASSIGNMENT_WORD {
		std::string var_name;
		STRING_EXPR var_val;
	};

	// '>' <word>
	struct REDIRECTION_OUT_FILE {
		STRING_EXPR file_name;
	};

	// '<' <word>
	struct REDIRECTION_IN_FILE {
		STRING_EXPR file_name;
	};

	// <number> '>' <word>
	struct REDIRECTION_NUMBER_OUT_FILE {
		int number;
		STRING_EXPR file_name;
	};

	// <number> '<' <word>
	struct REDIRECTION_NUMBER_IN_FILE {
		int number;
		STRING_EXPR file_name;
	};

	typedef boost::variant< REDIRECTION_OUT_FILE , REDIRECTION_IN_FILE , REDIRECTION_NUMBER_OUT_FILE , REDIRECTION_NUMBER_IN_FILE > redirection_type;

	struct REDIRECTION {
		redirection_type type;
	};

	/*
	 * Function declaration goes here...
	 */
	int parse(const std::string & inputString, REDIRECTION & toks);
}

#endif /* PARSER_H_ */
