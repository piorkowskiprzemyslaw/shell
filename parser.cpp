#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

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

int main(int argc, char **argv) {
	using boost::spirit::qi::int_;
    using boost::spirit::qi::parse;
	using boost::spirit::ascii::space;

    typedef parser<std::string::const_iterator> parser;

    parser g;

	std::vector<pipe_node> apps;

	std::string buffer;


	while(1) {
		std::cout << "$ ";
		apps.clear();
		getline(std::cin, buffer);
		std::string::const_iterator start = buffer.begin();
		std::string::const_iterator end = buffer.end();
		phrase_parse(start, end, g, space, apps);


		for(auto l : apps) {
			if(std::string *s = boost::get<std::string>(&l)) {
				std::cout << *s << std::endl;
			} 
			if(pipe_s *s = boost::get<pipe_s>(&l)) {
				std::cout << s->appname << " wyjscie do: " << s->filename << std::endl;
			}
		}
	}
	return 0;
}
