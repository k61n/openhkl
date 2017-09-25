#include <string>
#include <vector>

#include <Eigen/Dense>

#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/classic.hpp>

#include <nsxlib/utils/AffineTransformParser.h>
#include <nsxlib/utils/NSXTest.h>

const double tolerance=1e-6;
int main()
{
    std::string s("x-z+1/3,y-2x,z+3.4");

    Eigen::Transform<double,3,Eigen::Affine> m;
    nsx::AffineTransformParser<std::string::iterator> parser1;
    bool test=boost::spirit::qi::phrase_parse(s.begin(),s.end(),parser1,boost::spirit::qi::blank, m);

    // Validate parsing
    NSX_CHECK_ASSERT(test);
    // Compare
    NSX_CHECK_CLOSE(m(0,0),1,tolerance);
    NSX_CHECK_CLOSE(m(0,1),0,tolerance);
    NSX_CHECK_CLOSE(m(0,2),-1,tolerance);
    NSX_CHECK_CLOSE(m(0,3),1.0/3.0,tolerance);
    NSX_CHECK_CLOSE(m(1,0),-2,tolerance);
    NSX_CHECK_CLOSE(m(1,1),1,tolerance);
    NSX_CHECK_CLOSE(m(1,2),0,tolerance);
    NSX_CHECK_CLOSE(m(1,3),0,tolerance);
    NSX_CHECK_CLOSE(m(2,0),0,tolerance);
    NSX_CHECK_CLOSE(m(2,1),0,tolerance);
    NSX_CHECK_CLOSE(m(2,2),1,tolerance);
    NSX_CHECK_CLOSE(m(2,3),3.4,tolerance);
    NSX_CHECK_CLOSE(m(3,0),0,tolerance);
    NSX_CHECK_CLOSE(m(3,1),0,tolerance);
    NSX_CHECK_CLOSE(m(3,2),0,tolerance);
    NSX_CHECK_CLOSE(m(3,3),1,tolerance);

    return 0;
}
