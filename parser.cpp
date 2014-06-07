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

BOOST_FUSION_ADAPT_STRUCT(
    pipe_s,
    (std::string, appname)
    (std::string, filename)
)

typedef boost::variant<pipe_s, std::string> pipe_node;

template <typename Iterator>
struct parser : qi::grammar<Iterator, std::vector<pipe_node>(), ascii::space_type>
{
    parser() : parser::base_type(start)
    {
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;

		string %= lexeme[+(char_ - '|')];
		fpipe %= lexeme[+(char_ - char_(">|"))] >> ">>" >> string;
		fpipe_node %= fpipe | string;

		start %= 
			fpipe_node
			>> +('|' >> fpipe_node)
			;
    }

    qi::rule<Iterator, std::string(), ascii::space_type> string;
    qi::rule<Iterator, pipe_s(), qi::locals<std::string>, ascii::space_type> fpipe;
    qi::rule<Iterator, pipe_node(), ascii::space_type> fpipe_node;
    qi::rule<Iterator, std::vector<pipe_node>(), ascii::space_type> start;
};

void run(std::vector<pipe_node> &apps) {
	char *arg;

	int outfile = STDOUT_FILENO;
	if(pipe_s *s = boost::get<pipe_s>(&apps[0])) {
		arg = (char*)malloc(s->appname.size());
		s->appname.copy(arg, s->appname.size()-1); //na koncu jest spacja, a to bylo na szybko
		printf("%s\n", s->filename.c_str());
		outfile = open(s->filename.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
		if(outfile == -1)
			perror("open outfile: ");
	} 
	if(std::string *s = boost::get<std::string>(&apps[0])) {
		if(*s == "exit")
			exit(0);
		arg = (char*)malloc(s->size()+2);
		s->copy(arg, s->size()+1);
	} 

	process * p = ( process * ) malloc( sizeof(process) );
	job * j = ( job * ) malloc( sizeof(job) );
	j->pgid = 0 ;
	j->command = (char *)malloc(sizeof(char) * (strlen(arg)));

	for(size_t i = 0 ; i < strlen(arg) ; ++i){
		j->command[i] = arg[i];
	}

	j->next = NULL;
	j->first_process = p;
	j->notified = 0;
	j->stdin = STDIN_FILENO;
	//j->stdout = STDOUT_FILENO; /* może być też pfile jeśli chcemy zrobić przekierwoanie do pliku. */
	j->stdout = outfile;
	j->stderr = STDERR_FILENO;
	p->next = NULL;
	p->completed = 0;
	p->stopped = 0;
	p->pid = 0;
	p->status = 0;

	char * execArgs[] = { arg, NULL };
	p->argv = execArgs;

	job * actualJob;
	if(first_job != NULL) {
		for(actualJob = first_job ; actualJob->next ; actualJob = actualJob->next){ }
		actualJob->next = j;
	} else {
		first_job = j;
	}

	// jeśli proces ma być w background to drugim argumentem jest 0.
	launch_job(j, 1);
	if(outfile != STDOUT_FILENO) 
		close(outfile);
}

int main(int argc, char **argv) {
	using boost::spirit::qi::int_;
    using boost::spirit::qi::parse;
	using boost::spirit::ascii::space;

    typedef parser<std::string::const_iterator> parser;

    parser g;

	std::vector<pipe_node> apps;

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

		run(apps);
/*		for(auto l : apps) {
			if(std::string *s = boost::get<std::string>(&l)) {
				std::cout << *s << std::endl;
			} 
			if(pipe_s *s = boost::get<pipe_s>(&l)) {
				std::cout << s->appname << " wyjscie do: " << s->filename << std::endl;
				}
				}*/
	}
	return 0;
}
