#define BOOST_TEST_MODULE "Test Blob2D"
#define BOOST_TEST_DYN_LINK
#include "Blob2D.h"
#include <cmath>
#include <boost/test/unit_test.hpp>

using namespace SX::Geometry;

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_Blob2D)
{
	Blob2D blob;
	// Create a set of points for a 2D Gaussian.
	double c_x=12.0;
	double c_y=15.0;
	double sx2=2.0, sy2=3.0;
	double prefactor=0.5/(M_PI*sqrt(sx2*sy2));
	double tot=0;
	for (int i=0;i<50;++i)
	{
		for (int j=0;j<50;++j)
		{
			double mass=prefactor*exp(-0.5*(std::pow(i-c_x,2)/sx2+std::pow(j-c_y,2)/sy2));
			blob.addPoint(i,j,mass);
			tot+=mass;
		}
	}
	BOOST_CHECK_CLOSE(tot,1.0,tolerance);
	BOOST_CHECK_CLOSE(tot,blob.getMass(),tolerance);
	double xc,yc,sa,sb,angle;
	blob.toEllipse(xc,yc,sa,sb,angle);
	// Check that the center is OK
	BOOST_CHECK_CLOSE(xc,c_x,tolerance);
	BOOST_CHECK_CLOSE(yc,c_y,tolerance);
	// Check the semi_axes
	BOOST_CHECK_CLOSE(sa,sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(sb,sqrt(3.0),tolerance);
	BOOST_CHECK_CLOSE(angle,0.0,tolerance);

}
