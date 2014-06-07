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
#include <vector>
#include <memory>

namespace parser {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	/*
	 * Grammar stuff goes here...
	 */
	typedef std::string WORD;
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

	typedef boost::variant< WORD , NUMBER , BACK_QUOTE, QUOTE, BRACKETS, DOLLAR> node;

	struct tokens {
		std::vector<node> toks;
	};

	/*
	 * Function declaration goes here...
	 */
	int parse(const std::string & inputString, tokens & toks);
}

#endif /* PARSER_H_ */
