/*
 * interpreter.cpp
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#include <interpreter/interpreter.h>
#include <processManagement/processManagement.h>

namespace interpreter {

	void mini_shell_printer::operator()(const parser::SUBSHELL& subshell ) const
	{
		std::cout << "subshell: " << subshell.content << std::endl;
	}

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

	void mini_shell_printer::operator()(const parser::REDIRECTION_LIST& redirection_list) const
	{
		this->operator ()(redirection_list.redirection);
        BOOST_FOREACH(const parser::REDIRECTION_LIST_NODE& node, redirection_list.futher_redirections)
        {
            boost::apply_visitor(redirection_list_printer(), node);
        }
	}

	void mini_shell_printer::operator()(const parser::SIMPLE_COMMAND_ELEMENT& simple_command_element) const
	{
		boost::apply_visitor(simple_command_element_printer(), simple_command_element );
	}

	void mini_shell_printer::operator()(const parser::SIMPLE_COMMAND& simple_command) const
	{
		process * p = create_process_in_last_job();
		p->argv = (char**)calloc((simple_command.futher_simple_commands.size()+2)*sizeof(char*), 0);
		printf("tworzenie procesu\n");
		this->operator ()(simple_command.simple_command_element);
		BOOST_FOREACH(const parser::SIMPLE_COMMAND_NODE& node, simple_command.futher_simple_commands)
		{
			boost::apply_visitor(simple_command_printer(), node);
		}
	}

	void mini_shell_printer::operator()(const parser::COMMAND& command) const
	{
		boost::apply_visitor(command_type_printer(), command);
	}

	void mini_shell_printer::operator()(const parser::PIPELINE& pipeline) const
	{
		this->operator ()(pipeline.command);
		BOOST_FOREACH(const parser::PIPELINE_NODE& node, pipeline.pipeline_node)
		{
			boost::apply_visitor(pipeline_sequence_printer(), node);
		}
	}

	void mini_shell_printer::operator()(const parser::SIMPLE_LIST&  simple_list) const
	{
		this->operator ()(simple_list.pipeline);
		if(simple_list.separator){
			std::cout << "separator " << simple_list.separator.get() << std::endl;
		}
		BOOST_FOREACH(const parser::SIMPLE_LIST_NODE& node, simple_list.futher_pipes)
		{
			boost::apply_visitor(simple_list_node_printer(), node);
		}
	}


	void string_expr_printer::operator()(const parser::TEXT& text) const
	{
		printf("asdf\n");
		process *p = get_last_process();
		int ind = 0;
		while(p->argv[ind] != NULL)
			ind++;
		p->argv[ind] = (char*)malloc((text.size()+1)*sizeof(char));
		text.copy(p->argv[ind], text.size()); 
		p->argv[ind][text.size()] = 0;
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

	void redirection_list_printer::operator ()(const parser::REDIRECTION & redirection ) const
	{
		mini_shell_printer()(redirection);
	}

	void redirection_list_printer::operator ()(const parser::REDIRECTION_LIST & redirection_list ) const
	{
		mini_shell_printer()(redirection_list);
	}

	void simple_command_element_printer::operator()(const parser::STRING_EXPR& string_expr) const
	{
		boost::apply_visitor(string_expr_printer(), string_expr);
	}

	void simple_command_element_printer::operator()(const parser::ASSIGNMENT_WORD& assignment_word) const
	{
		mini_shell_printer()(assignment_word);
	}

	void simple_command_element_printer::operator()(const parser::REDIRECTION& redirection) const
	{
		mini_shell_printer()(redirection);
	}

	void simple_command_printer::operator ()(const parser::SIMPLE_COMMAND& simple_command) const
	{
		mini_shell_printer()(simple_command);
	}

	void simple_command_printer::operator ()(const parser::SIMPLE_COMMAND_ELEMENT& simple_command_element) const
	{
		mini_shell_printer()(simple_command_element);
	}

	void command_type_printer::operator ()(const parser::SIMPLE_COMMAND& simple_command) const
	{
		mini_shell_printer()(simple_command);
	}

	void command_type_printer::operator ()(const parser::SUBSHELL& subshell) const
	{
		mini_shell_printer()(subshell);
	}

	void command_type_printer::operator ()(const parser::SUBSHELL_REDIRECTION_LIST& subshell_redirection_list) const
	{
		mini_shell_printer()(subshell_redirection_list.subshell);
		mini_shell_printer()(subshell_redirection_list.redirection_list);
	}

	void pipeline_sequence_printer::operator() (const parser::COMMAND& command) const
	{
		mini_shell_printer()(command);
	}

	void pipeline_sequence_printer::operator()(const parser::PIPELINE& pipeline) const
	{
		mini_shell_printer()(pipeline);
	}

	void simple_list_node_printer::operator()(const parser::PIPELINE& pipeline) const
	{
		mini_shell_printer()(pipeline);
	}

	void simple_list_node_printer::operator()(const parser::SIMPLE_LIST& simple_list) const
	{
		mini_shell_printer()(simple_list);
	}
}

