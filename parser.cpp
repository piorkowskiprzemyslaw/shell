#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <string>

#include <util/globalVariables.h>
#include <util/initialization.h>
#include <processManagement/processManagement.h>
#include <variables/variables.h>
#include <commands/commands.h>


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

using qi::double_;
using qi::_1;
using ascii::space;
using phoenix::ref;

struct pipe_s
{
	std::string appname;
    std::string filename;
};
struct app;
typedef boost::variant<std::string, boost::recursive_wrapper<std::vector<app> > > arg_node;

struct app
{
	std::string appname;
	std::vector<arg_node> args;
};

BOOST_FUSION_ADAPT_STRUCT(
    pipe_s,
    (std::string, appname)
    (std::string, filename)
)

BOOST_FUSION_ADAPT_STRUCT(
	app,
	(std::string, appname)
	(std::vector<arg_node>, args)
)

//typedef boost::variant<pipe_s, app> pipe_node;

template <typename Iterator>
struct parser : qi::grammar<Iterator, std::vector<app>(), ascii::space_type>
{
    parser() : parser::base_type(app_group)
    {
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;


		using qi::on_error;
		using qi::on_success;
		using qi::fail;
		//using ascii::string;
		using namespace qi::labels;

		using phoenix::construct;
		using phoenix::val;
	
		string %= lexeme[+char_("a-zA-Z/.*")];

		fapp %= string >> args;
		args %= *farg_node;
		farg_node %= string | '`' >> app_group >> '`';
		app_group %= fapp >> *('|' >> fapp);

    }

 

    qi::rule<Iterator, std::string(), ascii::space_type> string;
	qi::rule<Iterator, arg_node(), ascii::space_type> farg_node;
    qi::rule<Iterator, std::vector<arg_node>(), ascii::space_type> args;
	qi::rule<Iterator, app(), ascii::space_type> fapp;
    qi::rule<Iterator, std::vector<app>(), ascii::space_type> app_group;
};

void run(std::vector<app> &apps) {
	char *arg;

	job * j = ( job * ) malloc( sizeof(job) );
	j->pgid = 0 ;
	j->command = (char *)malloc(sizeof(char) * (1));

	for(int i = 0 ; i < strlen(arg) ; ++i){
		//j->command[i] = arg[i];
		j->command[i] = 'a';
	}
	j->command[1] = 0;

	j->next = NULL;

	j->notified = 0;
	j->stdin = STDIN_FILENO;
	j->stdout = STDOUT_FILENO; /* może być też pfile jeśli chcemy zrobić przekierwoanie do pliku. */
	j->stderr = STDERR_FILENO;

	
	process * prev = NULL;
	for(auto l : apps) {
		if(l.appname == "exit")
			exit(0);
		process * p = ( process * ) malloc( sizeof(process) );
		p->next = NULL;
		p->completed = 0;
		p->stopped = 0;
		p->pid = 0;
		p->status = 0;

		p->argv = (char**)malloc((l.args.size()+2)*sizeof(char*));
		p->argv[l.args.size()+1] = NULL;
		if(prev != NULL) 
			prev->next = p;
		else  
			j->first_process = p;


		prev = p;
		p->argv[0] = (char*)malloc(l.appname.size()+1);
		l.appname.copy(p->argv[0], l.appname.size()); 
		int i = 1;
		for(auto s : l.args) {
			if(std::string *str = boost::get<std::string>(&s)) {
				p->argv[i] = (char*)malloc(str->size()+1);
				str->copy(p->argv[i++], str->size());
			} else if(std::vector<app> *a = boost::get<std::vector<app>>(&s))  {
			}

		}	
	}



	job * actualJob;
	if(first_job != NULL) {
		for(actualJob = first_job ; actualJob->next ; actualJob = actualJob->next){ }
		actualJob->next = j;
	} else {
		first_job = j;
	}

	// jeśli proces ma być w background to drugim argumentem jest 0.
	launch_job(j, 1);
}


void print_apps(std::vector<app> apps) {
	for(auto l : apps) {
		std::cout << l.appname << std::endl;
		for(auto s : l.args) {
			if(std::string *str = boost::get<std::string>(&s)) {
				std::cout << "  " << *str << std::endl;
			} else if(std::vector<app> *a = boost::get<std::vector<app>>(&s))  {
				std::cout << "----argument----" << std::endl;
				print_apps(*a);
				std::cout << "----------------" << std::endl;
			}
		}	
	}
}

int main(int argc, char **argv) {
	using boost::spirit::qi::int_;
    using boost::spirit::qi::parse;
	using boost::spirit::ascii::space;

    typedef parser<std::string::const_iterator> parser;

    parser g;

	std::vector<app> apps;

	std::string buffer;


	init();

	while(1) {
		refreshVariables();
		do_job_notification();
		std::cout << "$ ";
		apps.clear();
		getline(std::cin, buffer);
		std::string::const_iterator start = buffer.begin();
		std::string::const_iterator end = buffer.end();
		phrase_parse(start, end, g, space, apps);
		if(start != end) 
			std::cout << "not complete" << std::endl;

		print_apps(apps);
		run(apps);
	}
	return 0;
}
