/*
 * interpreter.cpp
 *
 *  Created on: 7 cze 2014
 *      Author: przemek
 */

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <interpreter/interpreter.h>
#include <processManagement/processManagement.h>
#include <variables/variables.h>

namespace interpreter {

	std::string expression_val;
	bool assignment_flag = false;

	void mini_shell_printer::operator()(const parser::SUBSHELL& subshell ) const
	{
		std::cout << "subshell: " << subshell.content << std::endl;
/*		char *shell_app = "./shell";
		process *p = (process*) malloc(sizeof(process));
		p->argv = (char**)malloc(sizeof(char*)*2);
		p->argv[0] = (char*)malloc(sizeof(char)*strlen(shell_app));
		p->argv[1] = 0;*/

	}

	void mini_shell_printer::operator ()(const parser::ASSIGNMENT_WORD & assignment) const
	{
		assignment_flag = true;
		boost::apply_visitor(string_expr_printer(), assignment.var_val);
		setVariable(assignment.var_name.c_str(), expression_val.c_str());
		expression_val.clear();
//		setVariable(assignment.var_name.c_str(), boost::get<std::string>(assignment.var_val).c_str());
		assignment_flag=false;

	}

	void mini_shell_printer::operator()(const parser::REDIRECTION& redirection) const
	{
//		std::cout << "redirection ";
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
		if(boost::get<parser::STRING_EXPR>(&simple_command.simple_command_element)) {
			process * p = create_process_in_last_job();

			p->argv = (char**)calloc((simple_command.futher_simple_commands.size()+2)*sizeof(char*), 1);
		}
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
			if(simple_list.separator.get() == "&") {
				job *j = first_job;
				while(j->next != NULL) {
					j = j->next;
				}
				j->foreground = 0;
			}
			else {
				std::cout << "separator " << simple_list.separator.get() << std::endl;
			}
		}
		BOOST_FOREACH(const parser::SIMPLE_LIST_NODE& node, simple_list.futher_pipes)
		{
			boost::apply_visitor(simple_list_node_printer(), node);
		}
	}


	void string_expr_printer::operator()(const parser::TEXT& text) const
	{
		//Set Expression
		if (assignment_flag)
		{
			expression_val = text;
			return;
		}

		process *p = get_last_process();
		int ind = 0;
		while(p->argv[ind] != NULL)
			ind++;

		p->argv[ind] = (char*)malloc((text.size()+1)*sizeof(char));
		text.copy(p->argv[ind], text.size());
		p->argv[ind][text.size()] = 0;

	}

	void string_expr_printer::operator()(const parser::NUMBER& number) const
	{
		expression_val = std::to_string(number);
		std::cout << "number: " << number << std::endl;
	}

	void string_expr_printer::operator()(const parser::BACK_QUOTE& back_quote) const
	{
		process *p = get_last_process();
		parser::SIMPLE_LIST tokens;
		parser::parse(back_quote.content,tokens);
		job * j = create_job();
		interpreter::mini_shell_printer interpreter;
		interpreter(tokens);
		int pipe[2];
		pipe2(pipe, O_NONBLOCK);
		j->stdout = pipe[1];
		char buffer[500];
		launch_job(j, 1);
		read(pipe[0], buffer, 500);
		int ind = 0;
		while(p->argv[ind] != NULL)
			ind++;
		p->argv[ind] = (char*)malloc((strlen(buffer)+1)*sizeof(char));
		strcpy(p->argv[ind], buffer);
	}

	void string_expr_printer::operator()(const parser::QUOTE& quote) const
	{
		expression_val = quote.content;
		std::cout << "quote: " << quote.content << std::endl;
	}

	void string_expr_printer::operator()(const parser::DOLLAR& dollar) const
	{
		const char * var = getVariable(dollar.content.c_str());

		if (assignment_flag)
		{
			if (var != NULL)
				expression_val = var;
			else
			{
				expression_val = "";
				std::cerr << "Unset variable." << std::endl;
			}
			return;
		}
		if(var == NULL) 
			var = "";

		process *p = get_last_process();
		int ind = 0;
		while(p->argv[ind] != NULL)
			ind++;
//		const char * var = getVariable(dollar.content.c_str());
		p->argv[ind] = (char*)malloc((strlen(var)+1)*sizeof(char));
		strcpy(p->argv[ind], var);
//		dollar.content.copy(p->argv[ind], dollar.content.size()); 
//		p->argv[ind][dollar.content.size()] = 0;
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_IN_FILE & redirection) const
	{
		int pfile = open(boost::get<std::string>(redirection.file_name).c_str(), O_RDONLY , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
		job * j = first_job;
		while(j->next != NULL) {
			j = j->next;
		}
		j->stdin = pfile;
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_OUT_FILE & redirection) const
	{
		int pfile = open(boost::get<std::string>(redirection.file_name).c_str(), O_CREAT | O_WRONLY , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
		job * j = first_job;
		while(j->next != NULL) {
			j = j->next;
		}
		j->stdout = pfile;
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_NUMBER_IN_FILE & redirection) const
	{
		std::cout << "redirection_number_in_file number : " << redirection.number << " file : ";
		boost::apply_visitor(string_expr_printer(), redirection.file_name);
	}

	void redirection_type_printer::operator()(const parser::REDIRECTION_NUMBER_OUT_FILE & redirection) const
	{
		int pfile = open(boost::get<std::string>(redirection.file_name).c_str(), O_CREAT | O_WRONLY , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
		job * j = first_job;
		while(j->next != NULL) {
			j = j->next;
		}
		if(redirection.number == 1) {
			j->stdout = pfile;
		} else if(redirection.number == 2) {
			j->stderr = pfile;
		}
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

