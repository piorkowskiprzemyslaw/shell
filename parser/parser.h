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
	struct SUBSHELL {
		std::string content;
	};
	struct DOLLAR {
		std::string content;
	};

	typedef boost::variant< TEXT , NUMBER , BACK_QUOTE, QUOTE, DOLLAR> STRING_EXPR;

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

	//<redirection> ::=  '>' <word>
    //				|  '<' <word>
    //				|  <number> '>' <word>
    //				|  <number> '<' <word>
	struct REDIRECTION {
		redirection_type type;
	};

	struct REDIRECTION_LIST;

	typedef boost::variant<boost::recursive_wrapper<REDIRECTION_LIST>, REDIRECTION> REDIRECTION_LIST_NODE;

	// <redirection_list> ::= <redirection> <redirection_list>
	//						| <redirection>
	struct REDIRECTION_LIST {
		REDIRECTION redirection;
		std::vector<REDIRECTION_LIST_NODE> futher_redirections;
	};

	typedef boost::variant< STRING_EXPR , ASSIGNMENT_WORD , REDIRECTION > SIMPLE_COMMAND_ELEMENT;

	struct SIMPLE_COMMAND;

	typedef boost::variant<boost::recursive_wrapper<SIMPLE_COMMAND>,  SIMPLE_COMMAND_ELEMENT> SIMPLE_COMMAND_NODE;

	// <simple_command> ::=  <simple_command_element> <simple_command>
    //					  |  <simple_command_element>
	struct SIMPLE_COMMAND {
		SIMPLE_COMMAND_ELEMENT simple_command_element;
		std::vector<SIMPLE_COMMAND_NODE> futher_simple_commands;
	};

	struct SUBSHELL_REDIRECTION_LIST {
		SUBSHELL subshell;
		REDIRECTION_LIST redirection_list;
	};

	// <command> ::=  <simple_command> | <subshell> | <subshell> <redirection_list>
	typedef boost::variant<SIMPLE_COMMAND , SUBSHELL_REDIRECTION_LIST , SUBSHELL> COMMAND;

	struct PIPELINE;

	typedef boost::variant< boost::recursive_wrapper<PIPELINE>, COMMAND > PIPELINE_NODE;

	// <pipeline> ::= <command> '|' <pipeline>
	//				| <pipeline>
	struct PIPELINE {
		COMMAND command;
		std::vector<PIPELINE_NODE> pipeline_node;
	};

	/*
	 * Function declaration goes here...
	 */
	int parse(const std::string & inputString, PIPELINE & toks);
}

#endif /* PARSER_H_ */
