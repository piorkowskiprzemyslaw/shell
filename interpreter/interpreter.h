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
		void operator()(const parser::ASSIGNMENT_WORD & assignment) const;
		void operator()(const parser::REDIRECTION & redirection) const;
	};

	struct string_expr_printer : boost::static_visitor<> {
		void operator()(const parser::TEXT& text) const;
		void operator()(const parser::NUMBER& number) const;
		void operator()(const parser::BACK_QUOTE& back_quote) const;
		void operator()(const parser::QUOTE& quote) const;
		void operator()(const parser::BRACKETS& brackets) const;
		void operator()(const parser::DOLLAR& dollar) const;
	};

	struct redirection_type_printer : boost::static_visitor<> {
		void operator()(const parser::REDIRECTION_IN_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_OUT_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_NUMBER_IN_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_NUMBER_OUT_FILE& redirection) const;
	};
}


#endif /* INTERPRETER_H_ */
