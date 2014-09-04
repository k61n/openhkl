#define BOOST_TEST_MODULE "Test Basis"
#define BOOST_TEST_DYN_LINK
#include "Basis.h"
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include <memory>

using namespace SX::Geometry;
using Eigen::Vector3d;
const double tolerance=1e-5;


BOOST_AUTO_TEST_CASE(Test_Basis)
{

	std::shared_ptr<Basis> bprime(new Basis(Vector3d(2,0,0),Vector3d(0,2,0),Vector3d(0,0,1)));

	Basis bsecond=Basis::fromDirectVectors(Vector3d(1,1,0),Vector3d(-1,1,0),Vector3d(0,0,1),bprime);

	Vector3d x(1,0,0);

	Vector3d xsecond=bsecond.fromStandardBasis(x);

	BOOST_CHECK_CLOSE(xsecond(0),0.25,tolerance);
	BOOST_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
	BOOST_CHECK_SMALL(xsecond(2),tolerance);

	x=bsecond.toStandardBasis(xsecond);

	BOOST_CHECK_CLOSE(x(0),1.0,tolerance);
	BOOST_CHECK_SMALL(x(1),tolerance);
	BOOST_CHECK_SMALL(x(2),tolerance);

	RowVector3d xr(1,0,0);
	RowVector3d xrsecond=bsecond.fromReciprocalStandardBasis(xr);

	BOOST_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
	BOOST_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
	BOOST_CHECK_SMALL(xrsecond(2),tolerance);

	xr=bsecond.toReciprocalStandardBasis(xrsecond);

	BOOST_CHECK_CLOSE(xr(0),1.0,tolerance);
	BOOST_CHECK_SMALL(xr(1),tolerance);
	BOOST_CHECK_SMALL(xr(2),tolerance);


}
