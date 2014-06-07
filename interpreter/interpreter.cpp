/*
 * interpreter.cpp
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#include <interpreter/interpreter.h>

namespace interpreter {

	void mini_shell_printer::operator ()(const parser::tokens & toks) const
	{
		BOOST_FOREACH(const parser::node & node, toks.toks)
		{
			boost::apply_visitor(mini_shell_tokens_printer(), node);
		}
	}

	void mini_shell_tokens_printer::operator()(const parser::tokens & toks) const
	{
		mini_shell_printer()(toks);
	}

	void mini_shell_tokens_printer::operator()(const parser::WORD& word) const
	{
		std::cout << "word: " << word << std::endl;
	}

	void mini_shell_tokens_printer::operator()(const parser::NUMBER& number) const
	{
		std::cout << "number: " << number << std::endl;
	}

	void mini_shell_tokens_printer::operator()(const parser::BACK_QUOTE& back_quote) const
	{
		std::cout << "back_quote: " << back_quote.content << std::endl;
	}

	void mini_shell_tokens_printer::operator()(const parser::QUOTE& quote) const
	{
		std::cout << "quote: " << quote.content << std::endl;
	}

	void mini_shell_tokens_printer::operator()(const parser::BRACKETS& brackets) const
	{
		std::cout << "brackets: " << brackets.content << std::endl;
	}

	void mini_shell_tokens_printer::operator()(const parser::DOLLAR& dollar) const
	{
		std::cout << "dollar: " << dollar.content << std::endl;
	}

}

