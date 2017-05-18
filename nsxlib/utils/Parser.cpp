#include <boost/spirit/include/qi.hpp>
#include "Parser.h"

namespace nsx {

void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    qi::phrase_parse(begin, end, *qi::int_ >> qi::eoi, ascii::space, v);
}

void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    qi::phrase_parse(begin, end, *qi::double_ >> qi::eoi, ascii::space, v);
}

} // end namespace nsx

