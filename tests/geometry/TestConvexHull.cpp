#define BOOST_TEST_MODULE "Test Convex Hull"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include "ConvexHull.h"

using namespace SX::Geometry;
using Eigen::Vector3d;
const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_ConvexHull)
{
	typedef Eigen::Vector3d vector3;

	// Create a convex hull
	SX::Geometry::ConvexHull<double> chull;

	// Fill it with a tetrahedron
	chull.makeVertex(vector3( 0, 0, 0));
	chull.makeVertex(vector3(10, 0, 0));
	chull.makeVertex(vector3( 0,10, 0));
	chull.makeVertex(vector3( 0, 0,10));

	chull.doubleTriangle();

	chull.constructHull();

	const auto& faces=chull.getFaces();
	const auto& edges=chull.getEdges();
	const auto& vertices=chull.getVertices();

	// Check that the number of vertices, edges and faces corresponds to a tetrahedron
	BOOST_CHECK_EQUAL(vertices.size(),4);
	BOOST_CHECK_EQUAL(edges.size(),6);
	BOOST_CHECK_EQUAL(faces.size(),4);

	// Fill the convex hull with new vertices in order to make a cube
	chull.makeVertex(vector3(10,10, 0));
	chull.makeVertex(vector3(10, 0,10));
	chull.makeVertex(vector3(0, 10,10));
	chull.makeVertex(vector3(10,10,10));

	// Update the hull
	chull.constructHull();

	// Check that the number of vertices, edges and faces corresponds to a cube
	BOOST_CHECK_EQUAL(vertices.size(),8);
	BOOST_CHECK_EQUAL(edges.size(),18);
	BOOST_CHECK_EQUAL(faces.size(),12);

}

