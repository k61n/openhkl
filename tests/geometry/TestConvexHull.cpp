#define BOOST_TEST_MODULE "Test Convex Hull"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "Error.h"
#include "Face.h"
#include "ConvexHull.h"

//! Check that the Hull satisfies the Euler conditions
template <typename T>
bool CheckEulerCondition(const SX::Geometry::ConvexHull<T>& chull)
{

	unsigned int nVertices=chull.getNVertices();
	unsigned int nEdges=chull.getNEdges();
	unsigned int nFaces=chull.getNFaces();

	std::cout<<"V="<<nVertices<<" E="<<nEdges<<" F="<<nFaces<<" ---> ";

	if (nVertices<4)
		return false;

	if ( (nVertices - nEdges + nFaces) != 2 )
		return false;
	else
		return true;

	if ( nFaces != (2 * nVertices - 4) )
		return false;
	else
		return true;

	if ( (2 * nEdges) != (3 * nFaces) )
		return false;
	else
		return true;
}

//! Check that the Hull satisfies the convexity condition. This consists in
//! checking that the signed volume between every face and every point is positive.
//! This shows that each point is inside every face and therefore the hull is convex.
template <typename T>
bool CheckConvexity(const SX::Geometry::ConvexHull<T>& chull)
{
	const auto& faces=chull.getFaces();
	const auto& vertices=chull.getVertices();

	for (auto & f: faces)
	{
		for (auto& v : vertices)
		{
			if (f->volumeSign(v) < 0)
				return false;
		}
	}
	return true;
}

using Eigen::Vector3d;
const double tolerance=1e-9;

BOOST_AUTO_TEST_CASE(Test_ConvexHull)
{
	typedef Eigen::Vector3d vector3;

	typedef SX::Geometry::ConvexHull<double> CHullDouble;

//	// Create an empty convex hull
//	CHullDouble chull;
//
//	// Checks that the hull can not be updated with 0 point to be processed
//	BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);
//
//	// Fill it with three vertices to form a tetrahedron
//	chull.addVertex(vector3( 0, 0, 0));
//	// Checks that the hull can not be updated with only 1 point to be processed
//	BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);
//
//	chull.addVertex(vector3(10, 0, 0));
//	// Checks that the hull can not be updated with only 2 point to be processed
//	BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);
//
//	chull.addVertex(vector3( 0,10, 0));
//	// Checks that the hull can not be updated with only 3 point to be processed
//	BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);
//
//	chull.addVertex(vector3( 0, 0,10));
//	// Checks that with 4 vertices the hull can be built
//	BOOST_CHECK_NO_THROW(chull.updateHull());
//
//	const auto& faces=chull.getFaces();
//	const auto& edges=chull.getEdges();
//	const auto& vertices=chull.getVertices();
//
//	// Check that the number of vertices, edges and faces corresponds to a tetrahedron
//	BOOST_CHECK_EQUAL(vertices.size(),4);
//	BOOST_CHECK_EQUAL(edges.size(),6);
//	BOOST_CHECK_EQUAL(faces.size(),4);
//
//	//! Checks that the hull satisfies the Euler conditions
//	BOOST_CHECK(CheckEulerCondition(chull));
//
//	//! Checks that the hull satisfies the Convexity condition
//	BOOST_CHECK(CheckConvexity(chull));
//
//	// Fill the convex hull with new vertices in order to make a cube
//	chull.addVertex(vector3(10,10, 0));
//	chull.addVertex(vector3(10, 0,10));
//	chull.addVertex(vector3(0, 10,10));
//	chull.addVertex(vector3(10,10,10));
//
//	// Update the hull
//	chull.updateHull();
//
//	// Check that the number of vertices, edges and faces corresponds to a cube
//	BOOST_CHECK_EQUAL(vertices.size(),8);
//	BOOST_CHECK_EQUAL(edges.size(),18);
//	BOOST_CHECK_EQUAL(faces.size(),12);
//
//	//! Checks that the hull satisfies the Euler conditions
//	BOOST_CHECK(CheckEulerCondition(chull));
//
//	//! Checks that the hull satisfies the Convexity condition
//	BOOST_CHECK(CheckConvexity(chull));
//
//	//! Checks that the volume of the cube is 10*10*10=1000
//	BOOST_CHECK_CLOSE(chull.getVolume(),1000,tolerance);

	CHullDouble chull1;

//	double r=10;
//	int nThetas=5;
//	int nPhis=5;
//	for (int i=1;i<nThetas;++i)
//	{
//		double z=r*cos(i*M_PI/nThetas);
//		for (int j=0;j<nPhis;++j)
//		{
//			double x=r*sin(i*M_PI/nThetas)*cos(j*M_2_PI/nPhis);
//			double y=r*sin(i*M_PI/nThetas)*sin(j*M_2_PI/nPhis);
//			chull1.addVertex(vector3(x,y,z));
//			try
//			{
//				chull1.updateHull();
//			}
//			catch(...)
//			{
//			}
//			std::cout<<CheckEulerCondition(chull1)<<" --- "<<x<<" "<<y<<" "<<z<<std::endl;
//		}
//	}

	chull1.addVertex(vector3( 0, 0,10));
	chull1.addVertex(vector3( 0, 0, 0));
	chull1.addVertex(vector3( 0,10, 0));
	chull1.addVertex(vector3(10,10, 0));
	chull1.addVertex(vector3(10, 0, 0));
	chull1.addVertex(vector3(15,15, 0));

	chull1.updateHull();
	std::cout<<CheckEulerCondition(chull1)<<std::endl;

}

