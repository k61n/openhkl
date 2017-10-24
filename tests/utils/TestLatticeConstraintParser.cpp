#include <string>

#include <nsxlib/LatticeConstraintParser.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/UtilsTypes.h>

int main()
{
    nsx::LatticeConstraintParser<std::string::iterator> parser;

    nsx::ConstraintSet constraints;

    std::string s("a=2b");
    NSX_CHECK_ASSERT(boost::spirit::qi::phrase_parse(s.begin(),s.end(),parser,boost::spirit::qi::blank,constraints));

    return 0;
}
