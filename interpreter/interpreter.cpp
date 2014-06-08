/*
 * interpreter.cpp
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#include <interpreter/interpreter.h>

namespace interpreter {

	void mini_shell_printer::operator ()(const parser::ASSIGNMENT_WORD & assignment) const
	{
		std::cout << "var_name = " << assignment.var_name << " var_val = ";
		boost::apply_visitor(string_expr_printer(), assignment.var_val);
	}

	void mini_shell_printer::operator()(const parser::REDIRECTION& redirection) const
	{
		std::cout << "redirection ";
		boost::apply_visitor(redirection_type_printer(), redirection.type);
	}

	void string_expr_printer::operator()(const parser::TEXT& text) const
	{
		std::cout << "text: " << text << std::endl;
	}

	void string_expr_printer::operator()(const parser::NUMBER& number) const
	{
		std::cout << "number: " << number << std::endl;
	}

	void string_expr_printer::operator()(const parser::BACK_QUOTE& back_quote) const
	{
		std::cout << "back_quote: " << back_quote.content << std::endl;
	}

	void string_expr_printer::operator()(const parser::QUOTE& quote) const
	{
		std::cout << "quote: " << quote.content << std::endl;
	}

	void string_expr_printer::operator()(const parser::BRACKETS& brackets) const
	{
		std::cout << "brackets: " << brackets.content << std::endl;
	}

	void string_expr_printer::operator()(const parser::DOLLAR& dollar) const
	{
		std::cout << "dollar: " << dollar.content << std::endl;
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_IN_FILE & redirection) const
	{
		std::cout << "redirection_in_file file : ";
		boost::apply_visitor(string_expr_printer(), redirection.file_name);
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_OUT_FILE & redirection) const
	{
		std::cout << "redirection_out_file file : ";
		boost::apply_visitor(string_expr_printer(), redirection.file_name);
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_NUMBER_IN_FILE & redirection) const
	{
		std::cout << "redirection_number_in_file number : " << redirection.number << " file : ";
		boost::apply_visitor(string_expr_printer(), redirection.file_name);
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_NUMBER_OUT_FILE & redirection) const
	{
		std::cout << "redirection_number_out_file number : " << redirection.number << " file : ";
		boost::apply_visitor(string_expr_printer(), redirection.file_name);
	}
}

