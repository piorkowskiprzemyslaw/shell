/*
 * interpreter.h
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <parser/parser.h>

namespace interpreter {

	struct mini_shell_printer {
		void operator()(const parser::tokens & toks) const;
	};

	struct mini_shell_tokens_printer : boost::static_visitor<> {
		void operator()(const parser::tokens & toks) const;
		void operator()(const parser::WORD& word) const;
		void operator()(const parser::NUMBER& number) const;
		void operator()(const parser::BACK_QUOTE& back_quote) const;
		void operator()(const parser::QUOTE& quote) const;
		void operator()(const parser::BRACKETS& brackets) const;
		void operator()(const parser::DOLLAR& dollar) const;
	};
}


#endif /* INTERPRETER_H_ */
