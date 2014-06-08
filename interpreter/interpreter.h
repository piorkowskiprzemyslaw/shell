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
		void operator()(const parser::SUBSHELL& subshell) const;
		void operator()(const parser::ASSIGNMENT_WORD & assignment) const;
		void operator()(const parser::REDIRECTION & redirection) const;
		void operator()(const parser::REDIRECTION_LIST & redirection) const;
		void operator()(const parser::SIMPLE_COMMAND_ELEMENT& simple_command_element) const;
		void operator()(const parser::SIMPLE_COMMAND& simple_command) const;
		void operator()(const parser::COMMAND& command) const;
		void operator()(const parser::PIPELINE& pipeline) const;
	};

	struct string_expr_printer : boost::static_visitor<> {
		void operator()(const parser::TEXT& text) const;
		void operator()(const parser::NUMBER& number) const;
		void operator()(const parser::BACK_QUOTE& back_quote) const;
		void operator()(const parser::QUOTE& quote) const;
		void operator()(const parser::DOLLAR& dollar) const;
	};

	struct redirection_type_printer : boost::static_visitor<> {
		void operator()(const parser::REDIRECTION_IN_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_OUT_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_NUMBER_IN_FILE& redirection) const;
		void operator()(const parser::REDIRECTION_NUMBER_OUT_FILE& redirection) const;
	};

	struct redirection_list_printer : boost::static_visitor<> {
		void operator()(const parser::REDIRECTION_LIST& redirection_list) const;
		void operator()(const parser::REDIRECTION& redirection) const;
	};

	struct simple_command_element_printer : boost::static_visitor<> {
		void operator()(const parser::STRING_EXPR& string_expr) const;
		void operator()(const parser::ASSIGNMENT_WORD& assignment_word) const;
		void operator()(const parser::REDIRECTION& redirection) const;
	};

	struct simple_command_printer : boost::static_visitor<> {
		void operator()(const parser::SIMPLE_COMMAND& simple_command) const;
		void operator()(const parser::SIMPLE_COMMAND_ELEMENT& simple_command_element) const;
	};

	struct command_type_printer : boost::static_visitor<> {
		void operator()(const parser::SIMPLE_COMMAND& simple_command) const;
		void operator()(const parser::SUBSHELL& subshell) const;
		void operator()(const parser::SUBSHELL_REDIRECTION_LIST& subshell_redirection_list) const;
	};

	struct pipeline_sequence_printer : boost::static_visitor<> {
		void operator()(const parser::COMMAND& command) const;
		void operator()(const parser::PIPELINE& pipeline) const;
	};
}


#endif /* INTERPRETER_H_ */
